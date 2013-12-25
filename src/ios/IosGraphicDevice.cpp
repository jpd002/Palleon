#include "IosGraphicDevice.h"
#include "athena/ios/IosVertexBuffer.h"
#include "athena/ios/IosTexture.h"
#include "athena/ios/IosEffect.h"
#include "athena/ios/IosUberEffectProvider.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"

using namespace Athena;

static const GLenum g_textureAddressModes[TEXTURE_ADDRESS_MODE_MAX] =
{
	GL_CLAMP_TO_EDGE,
	GL_REPEAT,
};

static const GLenum g_stencilOp[STENCIL_FAIL_ACTION_MAX] =
{
	GL_KEEP,
	GL_REPLACE
};

static const GLenum g_stencilFunc[STENCIL_FUNCTION_MAX] =
{
	GL_NEVER,
	GL_ALWAYS,
	GL_EQUAL
};

CIosGraphicDevice::CIosGraphicDevice(bool hasRetinaDisplay, const CVector2& screenSize)
: m_hasRetinaDisplay(hasRetinaDisplay)
{
	m_screenSize = screenSize;
	m_renderQueue.reserve(0x10000);
	m_defaultEffectProvider = std::make_shared<CIosUberEffectProvider>();
}

CIosGraphicDevice::~CIosGraphicDevice()
{

}

void CIosGraphicDevice::CreateInstance(bool hasRetinaDisplay, const CVector2& screenSize)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CIosGraphicDevice(hasRetinaDisplay, screenSize);
}

void CIosGraphicDevice::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}

void CIosGraphicDevice::Draw()
{
	if(m_hasRetinaDisplay)
	{
		glViewport(0, 0, m_screenSize.x * 2, m_screenSize.y * 2);
		glScissor(0, 0, m_screenSize.x * 2, m_screenSize.y * 2);
	}
	else
	{
		glViewport(0, 0, m_screenSize.x, m_screenSize.y);
		glScissor(0, 0, m_screenSize.x, m_screenSize.y);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Reset metrics
	m_drawCallCount = 0;
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_CULL_FACE);

	//Draw all viewports
	for(CViewport* viewport : m_viewports)
	{
		DrawViewport(viewport);
	}
	
	CHECKGLERROR();	
}

void CIosGraphicDevice::DrawViewport(CViewport* viewport)
{
	glClearDepthf(1.0f);
	glClearStencil(0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	CHECKGLERROR();
	
	CameraPtr camera = viewport->GetCamera();
	
	CMatrix4 projMatrix = camera->GetProjectionMatrix();
	CMatrix4 viewMatrix = camera->GetViewMatrix();
	
	m_viewProjMatrix = viewMatrix * projMatrix;
	
	viewMatrix(3, 0) = 0;
	viewMatrix(3, 1) = 0;
	viewMatrix(3, 2) = 0;
	
	m_peggedViewProjMatrix = viewMatrix * projMatrix;
	
	m_renderQueue.clear();
	
	const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(std::bind(&CIosGraphicDevice::FillRenderQueue, this, std::placeholders::_1, camera.get()));
	
	for(CMesh* mesh : m_renderQueue)
	{
		DrawMesh(mesh);
	}
}

VertexBufferPtr CIosGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	return VertexBufferPtr(new CIosVertexBuffer(bufferDesc));
}

TexturePtr CIosGraphicDevice::CreateTexture(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return CIosTexture::Create(textureFormat, width, height);
}

TexturePtr CIosGraphicDevice::CreateTextureFromFile(const char* path)
{
	return CIosTexture::CreateFromFile(path);
}

TexturePtr CIosGraphicDevice::CreateTextureFromMemory(const void* data, uint32 size)
{
	return CIosTexture::CreateFromMemory(data, size);
}

TexturePtr CIosGraphicDevice::CreateTextureFromRawData(const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	auto texture = CIosTexture::Create(textureFormat, width, height);
	std::static_pointer_cast<CIosTexture>(texture)->Update(data);
	return texture;
}

TexturePtr CIosGraphicDevice::CreateCubeTextureFromFile(const char* path)
{
	return CIosTexture::CreateCubeFromFile(path);
}

void CIosGraphicDevice::UpdateTexture(const TexturePtr& texture, const void* data)
{
	std::static_pointer_cast<CIosTexture>(texture)->Update(data);	
}

RenderTargetPtr CIosGraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return RenderTargetPtr();
}

CubeRenderTargetPtr CIosGraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CubeRenderTargetPtr();
}

bool CIosGraphicDevice::FillRenderQueue(const SceneNodePtr& node, CCamera* camera)
{
	if(!node->GetVisible()) return false;
	
	if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
	{
		m_renderQueue.push_back(mesh.get());
	}
	else if(auto meshProvider = std::dynamic_pointer_cast<CMeshProvider>(node))
	{
		meshProvider->GetMeshes(m_renderQueue, camera);
	}
	return true;
}

void CIosGraphicDevice::DrawMesh(CMesh* mesh)
{
	if(mesh->GetPrimitiveCount() == 0) return;
	
	CIosVertexBuffer* vertexBufferGen = static_cast<CIosVertexBuffer*>(mesh->GetVertexBuffer().get());
	assert(vertexBufferGen != NULL);
	
	GLuint vertexBuffer = vertexBufferGen->GetVertexBuffer();
	uint16* indexBuffer = vertexBufferGen->GetIndexBuffer();
	GLuint vertexArray = vertexBufferGen->GetVertexArray();
			
	auto effect = std::static_pointer_cast<CIosEffect>(mesh->GetEffectProvider()->GetEffectForRenderable(mesh, false));
	
	//Setup material
	{
		auto material = mesh->GetMaterial();
		assert(material);

		glUseProgram(effect->GetProgram());
		CHECKGLERROR();
		
		effect->UpdateConstants(material, mesh->GetWorldTransformation(), m_viewProjMatrix, CMatrix4());
		
		for(unsigned int i = 0; i < CMaterial::MAX_TEXTURE_SLOTS; i++)
		{
			const auto& texture = std::static_pointer_cast<CIosTexture>(material->GetTexture(i));
			if(!texture) continue;
			GLuint textureHandle = reinterpret_cast<GLuint>(texture->GetHandle());
			glActiveTexture(GL_TEXTURE0 + i);
			if(texture->IsCubeMap())
			{
				glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, textureHandle);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, g_textureAddressModes[material->GetTextureAddressModeU(i)]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, g_textureAddressModes[material->GetTextureAddressModeV(i)]);
			CHECKGLERROR();
		}
				
		ALPHA_BLENDING_MODE alphaBlendingMode = material->GetAlphaBlendingMode();
		if(alphaBlendingMode == ALPHA_BLENDING_LERP)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else if(alphaBlendingMode == ALPHA_BLENDING_ADD)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		CHECKGLERROR();
		
		if(material->GetStencilEnabled())
		{
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(g_stencilFunc[material->GetStencilFunction()], material->GetStencilValue(), ~0);
			glStencilOp(g_stencilOp[material->GetStencilFailAction()], GL_KEEP, GL_KEEP);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}
		CHECKGLERROR();
		
		glDepthMask(mesh->GetIsPeggedToOrigin() ? GL_FALSE : GL_TRUE);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	CHECKGLERROR();
	
	glBindVertexArrayOES(vertexArray);
	CHECKGLERROR();
	
	GLenum primitiveType = GL_TRIANGLES;
	GLsizei primitiveCount = mesh->GetPrimitiveCount();
	GLsizei vertexCount = primitiveCount * 3;
	switch(mesh->GetPrimitiveType())
	{
		case PRIMITIVE_TRIANGLE_STRIP:
			primitiveType = GL_TRIANGLE_STRIP;
			vertexCount = (primitiveCount + 2);
			break;
		case PRIMITIVE_TRIANGLE_LIST:
			primitiveType = GL_TRIANGLES;
			vertexCount = (primitiveCount * 3);
			break;
	}
	
	glDrawElements(primitiveType, vertexCount, GL_UNSIGNED_SHORT, indexBuffer);
	CHECKGLERROR();
	
	m_drawCallCount++;
}

void CIosGraphicDevice::SetFrameRate(float frameRate)
{
	m_frameRate = frameRate;
}

uint32 CGraphicDevice::ConvertColorToUInt32(const CColor& color)
{
	return 
		(static_cast<uint32>(color.a * 255.f) << 24) |
		(static_cast<uint32>(color.r * 255.f) <<  0) |
		(static_cast<uint32>(color.g * 255.f) <<  8) |
		(static_cast<uint32>(color.b * 255.f) << 16);
}
