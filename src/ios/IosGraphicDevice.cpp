#include "IosGraphicDevice.h"
#include "athena/ios/IosVertexBuffer.h"
#include "athena/ios/IosTexture.h"
#include "athena/ios/IosEffect.h"
#include "athena/ios/IosUberEffectProvider.h"
#include "athena/ios/IosShadowMapEffect.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"

#define SHADOW_MAP_SIZE (1024)

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
	CreateShadowMap();
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

void CIosGraphicDevice::SetMainFramebuffer(GLuint mainFramebuffer)
{
	m_mainFramebuffer = mainFramebuffer;
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
	glClearDepthf(1.0f);
	glClearStencil(0xFF);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	//Reset metrics
	m_drawCallCount = 0;
	
	//TODO: Move these state changes in material setup before drawing
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
	DrawViewportShadowMap(viewport);
	DrawViewportMainMap(viewport);
}

void CIosGraphicDevice::DrawViewportMainMap(CViewport* viewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_mainFramebuffer);

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
	
	glDepthMask(GL_TRUE);
	glClearDepthf(1.0f);
	glClearStencil(0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	CHECKGLERROR();
	
	auto camera = viewport->GetCamera();
	auto shadowCamera = viewport->GetShadowCamera();
			
//	viewMatrix(3, 0) = 0;
//	viewMatrix(3, 1) = 0;
//	viewMatrix(3, 2) = 0;
	
//	m_peggedViewProjMatrix = viewMatrix * projMatrix;
	
	m_renderQueue.clear();
	
	const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(std::bind(&CIosGraphicDevice::FillRenderQueue, this, std::placeholders::_1, camera.get()));
	
	auto viewProjMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	auto shadowViewProjMatrix = shadowCamera ? (shadowCamera->GetViewMatrix() * shadowCamera->GetProjectionMatrix()) : CMatrix4::MakeIdentity();
	bool hasShadowMap = shadowCamera != nullptr;
	for(CMesh* mesh : m_renderQueue)
	{
		auto effectProvider = mesh->GetEffectProvider();
		auto effect = std::static_pointer_cast<CIosEffect>(effectProvider->GetEffectForRenderable(mesh, hasShadowMap));
		DrawMesh(mesh, effect, viewProjMatrix, hasShadowMap, shadowViewProjMatrix);
	}
}

void CIosGraphicDevice::DrawViewportShadowMap(CViewport* viewport)
{
	auto camera = viewport->GetShadowCamera();
	if(!camera) return;
		
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer);
	
	if(!m_shadowMapEffect)
	{
		m_shadowMapEffect = std::make_shared<CIosShadowMapEffect>();
	}
	
	glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	glScissor(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_renderQueue.clear();
	
	const auto& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&](const SceneNodePtr& node)
		{
			if(!node->GetVisible()) return false;

			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(mesh->GetMaterial()->GetShadowCasting())
				{
					m_renderQueue.push_back(mesh.get());
				}
			}
			return true;
		}
	);
	
	auto viewProjMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	for(const auto& mesh : m_renderQueue)
	{
		DrawMesh(mesh, m_shadowMapEffect, viewProjMatrix);
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

void CIosGraphicDevice::DrawMesh(CMesh* mesh, const IosEffectPtr& effect, const CMatrix4& viewProjMatrix, bool hasShadowMap, const CMatrix4& shadowViewProjMatrix)
{
	if(mesh->GetPrimitiveCount() == 0) return;
	
	CIosVertexBuffer* vertexBufferGen = static_cast<CIosVertexBuffer*>(mesh->GetVertexBuffer().get());
	assert(vertexBufferGen != NULL);
	
	GLuint vertexBuffer = vertexBufferGen->GetVertexBuffer();
	uint16* indexBuffer = vertexBufferGen->GetIndexBuffer();
	GLuint vertexArray = vertexBufferGen->GetVertexArray();

	//Setup material
	{
		auto material = mesh->GetMaterial();
		assert(material);

		glUseProgram(effect->GetProgram());
		CHECKGLERROR();
		
		effect->UpdateConstants(material, mesh->GetWorldTransformation(), viewProjMatrix, shadowViewProjMatrix);
		CHECKGLERROR();

		unsigned int textureCount = 0;
		for(unsigned int i = 0; i < CMaterial::MAX_TEXTURE_SLOTS; i++)
		{
			const auto& texture = std::static_pointer_cast<CIosTexture>(material->GetTexture(i));
			if(!texture) break;
			GLuint textureHandle = static_cast<GLuint>(reinterpret_cast<size_t>(texture->GetHandle()));
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
			textureCount++;
		}
		
		if(hasShadowMap)
		{
			glActiveTexture(GL_TEXTURE0 + textureCount);
			glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			CHECKGLERROR();			
			textureCount++;
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
		default:
			assert(0);
			break;
	}
	
	glDrawElements(primitiveType, vertexCount, GL_UNSIGNED_SHORT, indexBuffer);
	CHECKGLERROR();
	
	m_drawCallCount++;
}

void CIosGraphicDevice::CreateShadowMap()
{
	glGenTextures(1, &m_shadowMapTexture);
	
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_RGBA, GL_HALF_FLOAT_OES, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECKGLERROR();
	
	glGenRenderbuffers(1, &m_shadowMapDepthbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, m_shadowMapDepthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	CHECKGLERROR();

	glGenFramebuffers(1, &m_shadowMapFramebuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_shadowMapTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_shadowMapDepthbuffer);
	CHECKGLERROR();
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch(status)
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				NSLog(@"Failed to build framebuffer: Incomplete Attachment.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				NSLog(@"Failed to build framebuffer: Incomplete Dimensions.");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				NSLog(@"Failed to build framebuffer: Missing Attachment.");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				NSLog(@"Failed to build framebuffer: Unsupported.");
				break;
			default:
				NSLog(@"Failed to build framebuffer: Unknown Status(0x%x).", status);
				break;
		}
		assert(0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
