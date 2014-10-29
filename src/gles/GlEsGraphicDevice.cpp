#include "palleon/gles/GlEsGraphicDevice.h"
#include "palleon/gles/GlEsVertexBuffer.h"
#include "palleon/gles/GlEsTexture.h"
#include "palleon/gles/GlEsRenderTarget.h"
#include "palleon/gles/GlEsEffect.h"
#include "palleon/gles/GlEsUberEffectProvider.h"
#include "palleon/gles/GlEsShadowMapEffect.h"
#include "palleon/graphics/Mesh.h"
#include "palleon/graphics/MeshProvider.h"
#include "palleon/Log.h"

#define SHADOW_MAP_SIZE (1024)

using namespace Palleon;

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

CGlEsGraphicDevice::CGlEsGraphicDevice(const CVector2& screenSize, float dpiScale)
{
	m_screenSize = screenSize;
	m_scaledScreenSize = screenSize * dpiScale;
	m_renderQueue.reserve(0x10000);
	m_defaultEffectProvider = std::make_shared<CGlEsUberEffectProvider>();
}

CGlEsGraphicDevice::~CGlEsGraphicDevice()
{

}

void CGlEsGraphicDevice::Initialize()
{
	auto extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
	m_hasVertexArrayObjects = strstr(extensions, "GL_OES_vertex_array_object") != nullptr;
	CreateShadowMap();
}

void CGlEsGraphicDevice::SetMainFramebuffer(GLuint mainFramebuffer)
{
	m_mainFramebuffer = mainFramebuffer;
}

void CGlEsGraphicDevice::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_mainFramebuffer);

	glViewport(0, 0, m_scaledScreenSize.x, m_scaledScreenSize.y);
	glScissor(0, 0, m_scaledScreenSize.x, m_scaledScreenSize.y);

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
	
	//Make sure buffers don't leak somewhere else
	if(m_hasVertexArrayObjects)
	{
#ifdef GL_ES_VERSION_3_0
		glBindVertexArray(0);
#else
		glBindVertexArrayOES(0);
#endif
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECKGLERROR();
}

void CGlEsGraphicDevice::DrawViewport(CViewport* viewport)
{
	DrawViewportShadowMap(viewport);

	glBindFramebuffer(GL_FRAMEBUFFER, m_mainFramebuffer);
	DrawViewportMainMap(viewport, m_scaledScreenSize.x, m_scaledScreenSize.y);
}

void CGlEsGraphicDevice::DrawViewportMainMap(CViewport* viewport, uint32 width, uint32 height)
{
	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);
	
	glDepthMask(GL_TRUE);
	glClearDepthf(1.0f);
	glClearStencil(0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	CHECKGLERROR();
	
	auto camera = viewport->GetCamera();
	auto shadowCamera = viewport->GetShadowCamera();
	
	m_renderQueue.clear();
	
	const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(std::bind(&CGlEsGraphicDevice::FillRenderQueue, this, std::placeholders::_1, camera.get()));
	
	auto viewMatrix = camera->GetViewMatrix();
	auto shadowViewProjMatrix = shadowCamera ? (shadowCamera->GetViewMatrix() * shadowCamera->GetProjectionMatrix()) : CMatrix4::MakeIdentity();
	bool hasShadowMap = shadowCamera != nullptr;
	
	auto peggedViewMatrix = camera->GetViewMatrix();
	peggedViewMatrix(3, 0) = 0;
	peggedViewMatrix(3, 1) = 0;
	peggedViewMatrix(3, 2) = 0;
	
	VIEWPORT_PARAMS viewportParams;
	viewportParams.viewport = viewport;
	viewportParams.projMatrix = camera->GetProjectionMatrix();
	viewportParams.hasShadowMap = hasShadowMap;
	viewportParams.shadowViewProjMatrix = shadowViewProjMatrix;
	for(const auto& mesh : m_renderQueue)
	{
		auto effectProvider = mesh->GetEffectProvider();
		auto effect = std::static_pointer_cast<CGlEsEffect>(effectProvider->GetEffectForRenderable(mesh, hasShadowMap));
		bool isPeggedToOrigin = mesh->GetIsPeggedToOrigin();
		viewportParams.viewMatrix = isPeggedToOrigin ? peggedViewMatrix : viewMatrix;
		DrawMesh(viewportParams, mesh, effect);
	}
}

void CGlEsGraphicDevice::DrawViewportShadowMap(CViewport* viewport)
{
	auto camera = viewport->GetShadowCamera();
	if(!camera) return;

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFramebuffer);
	
	if(!m_shadowMapEffect)
	{
		m_shadowMapEffect = std::make_shared<CGlEsShadowMapEffect>();
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
	
	VIEWPORT_PARAMS viewportParams;
	viewportParams.viewport = viewport;
	viewportParams.projMatrix = camera->GetProjectionMatrix();
	viewportParams.viewMatrix = camera->GetViewMatrix();
	for(const auto& mesh : m_renderQueue)
	{
		DrawMesh(viewportParams, mesh, m_shadowMapEffect);
	}
}

VertexBufferPtr CGlEsGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	return std::make_shared<CGlEsVertexBuffer>(bufferDesc, m_hasVertexArrayObjects);
}

TexturePtr CGlEsGraphicDevice::CreateTexture(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height, uint32 mipCount)
{
	return CGlEsTexture::Create(textureFormat, width, height);
}

TexturePtr CGlEsGraphicDevice::CreateCubeTexture(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CGlEsTexture::CreateCube(textureFormat, size);
}

RenderTargetPtr CGlEsGraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return std::make_shared<CGlEsRenderTarget>(textureFormat, width, height);
}

CubeRenderTargetPtr CGlEsGraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CubeRenderTargetPtr();
}

bool CGlEsGraphicDevice::FillRenderQueue(const SceneNodePtr& node, CCamera* camera)
{
	if(!node->GetVisible()) return false;
	
	if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
	{
		if(mesh->GetPrimitiveCount() != 0)
		{
			m_renderQueue.push_back(mesh.get());
		}
	}
	else if(auto meshProvider = std::dynamic_pointer_cast<CMeshProvider>(node))
	{
		meshProvider->GetMeshes(m_renderQueue, camera);
	}
	return true;
}

void CGlEsGraphicDevice::DrawMesh(const VIEWPORT_PARAMS& viewportParams, CMesh* mesh, const GlEsEffectPtr& effect)
{
	if(mesh->GetPrimitiveCount() == 0) return;
	
	CGlEsVertexBuffer* vertexBufferGen = static_cast<CGlEsVertexBuffer*>(mesh->GetVertexBuffer().get());
	assert(vertexBufferGen != NULL);
	
	//Setup material
	{
		auto material = mesh->GetMaterial();
		assert(material);

		glUseProgram(effect->GetProgram());
		CHECKGLERROR();
		
		effect->UpdateConstants(viewportParams, material.get(), mesh->GetWorldTransformation());
		CHECKGLERROR();

		unsigned int textureCount = 0;
		for(unsigned int i = 0; i < CMaterial::MAX_TEXTURE_SLOTS; i++)
		{
			const auto& texture = std::static_pointer_cast<CGlEsTexture>(material->GetTexture(i));
			if(!texture) continue;
			GLuint textureHandle = static_cast<GLuint>(reinterpret_cast<size_t>(texture->GetHandle()));
			glActiveTexture(GL_TEXTURE0 + i);
			GLenum target = texture->IsCube() ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
			glBindTexture(target, textureHandle);
			glTexParameteri(target, GL_TEXTURE_WRAP_S, g_textureAddressModes[material->GetTextureAddressModeU(i)]);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, g_textureAddressModes[material->GetTextureAddressModeV(i)]);
			CHECKGLERROR();
			textureCount++;
		}
		
		if(viewportParams.hasShadowMap)
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
	
	if(m_hasVertexArrayObjects)
	{
		GLuint vertexArray = vertexBufferGen->GetVertexArray();
#ifdef GL_ES_VERSION_3_0
		glBindVertexArray(vertexArray);
#else
		glBindVertexArrayOES(vertexArray);
#endif
		CHECKGLERROR();
	}
	else
	{
		vertexBufferGen->BindBuffers();
	}
	
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
		case PRIMITIVE_LINE_LIST:
			primitiveType = GL_LINES;
			vertexCount = (primitiveCount * 2);
			break;
		default:
			assert(0);
			break;
	}
	
#ifdef _DEBUG
	effect->ValidateProgram();
#endif
	
	glDrawElements(primitiveType, vertexCount, GL_UNSIGNED_SHORT, nullptr);
	CHECKGLERROR();
	
	m_drawCallCount++;
}

void CGlEsGraphicDevice::CreateShadowMap()
{
	glGenTextures(1, &m_shadowMapTexture);
	
	glBindTexture(GL_TEXTURE_2D, m_shadowMapTexture);

#ifdef GL_RED_EXT		//Only works on iOS
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_RED_EXT, GL_HALF_FLOAT_OES, nullptr);
#else
	glTexImage2D(GL_TEXTURE_2D, 0 ,GL_RGBA, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0, GL_RGBA, GL_FLOAT, nullptr);
#endif
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
				CLog::GetInstance().Print("Failed to build framebuffer: Incomplete Attachment.\r\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				CLog::GetInstance().Print("Failed to build framebuffer: Incomplete Dimensions.\r\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				CLog::GetInstance().Print("Failed to build framebuffer: Missing Attachment.\r\n");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				CLog::GetInstance().Print("Failed to build framebuffer: Unsupported.\r\n");
				break;
			default:
				CLog::GetInstance().Print("Failed to build framebuffer: Unknown Status(0x%x).\r\n", status);
				break;
		}
		assert(0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CGlEsGraphicDevice::SetFrameRate(float frameRate)
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
