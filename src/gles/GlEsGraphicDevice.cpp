#include "palleon/gles/GlEsGraphicDevice.h"
#include "palleon/gles/GlEsVertexBuffer.h"
#include "palleon/gles/GlEsTexture.h"
#include "palleon/gles/GlEsEffect.h"
#include "palleon/gles/GlEsUberEffectProvider.h"
#include "palleon/gles/GlEsShadowMapEffect.h"
#include "palleon/Mesh.h"
#include "palleon/MeshProvider.h"

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
: m_scaledScreenSize(screenSize * dpiScale)
{
	m_screenSize = screenSize;
	m_renderQueue.reserve(0x10000);
	m_defaultEffectProvider = std::make_shared<CGlEsUberEffectProvider>();
}

CGlEsGraphicDevice::~CGlEsGraphicDevice()
{

}

void CGlEsGraphicDevice::Initialize()
{
	CreateShadowMap();
}

void CGlEsGraphicDevice::SetMainFramebuffer(GLuint mainFramebuffer)
{
	m_mainFramebuffer = mainFramebuffer;
}

void CGlEsGraphicDevice::Draw()
{
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
	
	//Make sure VAO doesn't leak somewhere else
	glBindVertexArrayOES(0);
	CHECKGLERROR();
}

void CGlEsGraphicDevice::DrawViewport(CViewport* viewport)
{
	DrawViewportShadowMap(viewport);
	DrawViewportMainMap(viewport);
}

void CGlEsGraphicDevice::DrawViewportMainMap(CViewport* viewport)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_mainFramebuffer);

	glViewport(0, 0, m_scaledScreenSize.x, m_scaledScreenSize.y);
	glScissor(0, 0, m_scaledScreenSize.x, m_scaledScreenSize.y);
	
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
	auto projMatrix = camera->GetProjectionMatrix();
	auto shadowViewProjMatrix = shadowCamera ? (shadowCamera->GetViewMatrix() * shadowCamera->GetProjectionMatrix()) : CMatrix4::MakeIdentity();
	bool hasShadowMap = shadowCamera != nullptr;
	
	auto peggedViewMatrix = camera->GetViewMatrix();
	peggedViewMatrix(3, 0) = 0;
	peggedViewMatrix(3, 1) = 0;
	peggedViewMatrix(3, 2) = 0;
	
	for(const auto& mesh : m_renderQueue)
	{
		auto effectProvider = mesh->GetEffectProvider();
		auto effect = std::static_pointer_cast<CGlEsEffect>(effectProvider->GetEffectForRenderable(mesh, hasShadowMap));
		bool isPeggedToOrigin = mesh->GetIsPeggedToOrigin();
		DrawMesh(mesh, effect, isPeggedToOrigin ? peggedViewMatrix : viewMatrix, projMatrix, hasShadowMap, shadowViewProjMatrix);
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
	
	auto viewMatrix = camera->GetViewMatrix();
	auto projMatrix = camera->GetProjectionMatrix();
	for(const auto& mesh : m_renderQueue)
	{
		DrawMesh(mesh, m_shadowMapEffect, viewMatrix, projMatrix);
	}
}

VertexBufferPtr CGlEsGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	return VertexBufferPtr(new CGlEsVertexBuffer(bufferDesc));
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
	return RenderTargetPtr();
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

void CGlEsGraphicDevice::DrawMesh(CMesh* mesh, const GlEsEffectPtr& effect, const CMatrix4& viewMatrix, const CMatrix4& projMatrix, bool hasShadowMap, const CMatrix4& shadowViewProjMatrix)
{
	if(mesh->GetPrimitiveCount() == 0) return;
	
	CGlEsVertexBuffer* vertexBufferGen = static_cast<CGlEsVertexBuffer*>(mesh->GetVertexBuffer().get());
	assert(vertexBufferGen != NULL);
	
	GLuint vertexArray = vertexBufferGen->GetVertexArray();

	//Setup material
	{
		auto material = mesh->GetMaterial();
		assert(material);

		glUseProgram(effect->GetProgram());
		CHECKGLERROR();
		
		effect->UpdateConstants(material, mesh->GetWorldTransformation(), viewMatrix, projMatrix, shadowViewProjMatrix);
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
		case PRIMITIVE_LINE_LIST:
			primitiveType = GL_LINES;
			vertexCount = (primitiveCount * 2);
			break;
		default:
			assert(0);
			break;
	}
	
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
				printf("Failed to build framebuffer: Incomplete Attachment.\r\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				printf("Failed to build framebuffer: Incomplete Dimensions.\r\n");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				printf("Failed to build framebuffer: Missing Attachment.\r\n");
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				printf("Failed to build framebuffer: Unsupported.\r\n");
				break;
			default:
				printf("Failed to build framebuffer: Unknown Status(0x%x).\r\n", status);
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
