#include "palleon/ios/MetalGraphicDevice.h"
#include "palleon/ios/MetalVertexBuffer.h"
#include "palleon/ios/MetalTexture.h"
#include "palleon/ios/MetalUberEffectProvider.h"
#include "palleon/MeshProvider.h"

using namespace Palleon;

CMetalGraphicDevice::CMetalGraphicDevice(MetalView* metalView)
: m_metalView(metalView)
{
	m_screenSize = CVector2(480, 320);
	m_commandQueue = [m_metalView.device newCommandQueue];
	m_mainRenderPass = [[MTLRenderPassDescriptor renderPassDescriptor] retain];
	m_mainAdditionalRenderPass = [[MTLRenderPassDescriptor renderPassDescriptor] retain];
	m_constantBuffer = [m_metalView.device newBufferWithLength: CONSTANT_BUFFER_SIZE options: MTLResourceOptionCPUCacheModeDefault];
	m_defaultEffectProvider = std::make_shared<CMetalUberEffectProvider>(m_metalView.device);
	m_shadowMapEffectProvider = std::make_shared<CMetalShadowMapEffectProvider>(m_metalView.device);
	m_drawSemaphore = dispatch_semaphore_create(0);
	
	@autoreleasepool
	{
		CreateDepthStencilState();
		CreateShadowMap();
	}
}

CMetalGraphicDevice::~CMetalGraphicDevice()
{
	//TODO: Cleanup
	assert(m_samplerStates.empty());
	[m_depthStencilState release];
	[m_shadowMap release];
	[m_constantBuffer release];
	[m_commandQueue release];
	[m_mainRenderPass release];
	[m_mainAdditionalRenderPass release];
}

void CMetalGraphicDevice::CreateInstance(MetalView* metalView)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CMetalGraphicDevice(metalView);
}

void CMetalGraphicDevice::DestroyInstance()
{
	assert(m_instance != nullptr);
	if(m_instance == nullptr) return;
	delete m_instance;
}

void CMetalGraphicDevice::CreateDepthStencilState()
{
	MTLDepthStencilDescriptor* depthStateDesc = [[MTLDepthStencilDescriptor alloc] init];
	depthStateDesc.depthCompareFunction = MTLCompareFunctionLessEqual;
	depthStateDesc.depthWriteEnabled = YES;
	m_depthStencilState = [m_metalView.device newDepthStencilStateWithDescriptor: depthStateDesc];
	[depthStateDesc release];
}

void CMetalGraphicDevice::CreateShadowMap()
{
	static const unsigned int shadowMapSize = 1024;
	
	{
		MTLTextureDescriptor* textureDesc = [[MTLTextureDescriptor alloc] init];
		textureDesc.width = shadowMapSize;
		textureDesc.height = shadowMapSize;
		textureDesc.mipmapLevelCount = 1;
		textureDesc.pixelFormat = MTLPixelFormatDepth32Float;
		m_shadowMap = [m_metalView.device newTextureWithDescriptor: textureDesc];
		assert(m_shadowMap != nil);
		[textureDesc release];
	}
	
	m_shadowRenderPass = [[MTLRenderPassDescriptor renderPassDescriptor] retain];
	m_shadowRenderPass.depthAttachment.texture = m_shadowMap;
	m_shadowRenderPass.depthAttachment.loadAction = MTLLoadActionClear;
	m_shadowRenderPass.depthAttachment.clearDepth = 1.0f;
	m_shadowRenderPass.depthAttachment.storeAction = MTLStoreActionStore;
}

VertexBufferPtr CMetalGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	return std::make_shared<CMetalVertexBuffer>(m_metalView.device, bufferDesc);
}

TexturePtr CMetalGraphicDevice::CreateTexture(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height, uint32 mipCount)
{
	return CMetalTexture::Create(m_metalView.device, textureFormat, width, height);
}

TexturePtr CMetalGraphicDevice::CreateCubeTexture(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return TexturePtr();
}

RenderTargetPtr CMetalGraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return RenderTargetPtr();
}

CubeRenderTargetPtr CMetalGraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CubeRenderTargetPtr();
}

void CMetalGraphicDevice::SetupMainRenderPass(id<CAMetalDrawable> drawable)
{
	if(!m_mainDepthBuffer || (m_mainDepthBuffer && (m_mainDepthBuffer.width != drawable.texture.width || m_mainDepthBuffer.height != drawable.texture.height)))
	{
		MTLTextureDescriptor* desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat: MTLPixelFormatDepth32Float width: drawable.texture.width height: drawable.texture.height mipmapped: NO];
		m_mainDepthBuffer = [m_metalView.device newTextureWithDescriptor: desc];
		m_mainDepthBuffer.label = @"Main Depth Buffer";
	}

	{
		m_mainRenderPass.colorAttachments[0].texture = drawable.texture;
		m_mainRenderPass.colorAttachments[0].loadAction = MTLLoadActionClear;
		m_mainRenderPass.colorAttachments[0].clearColor = MTLClearColorMake(0.0f, 0.0f, 0.0f, 0.0f);
		m_mainRenderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
		
		m_mainRenderPass.depthAttachment.texture = m_mainDepthBuffer;
		m_mainRenderPass.depthAttachment.loadAction = MTLLoadActionClear;
		m_mainRenderPass.depthAttachment.clearDepth = 1.0f;
		m_mainRenderPass.depthAttachment.storeAction = MTLStoreActionDontCare;
	}
	
	{
		m_mainAdditionalRenderPass.colorAttachments[0].texture = drawable.texture;
		m_mainAdditionalRenderPass.colorAttachments[0].loadAction = MTLLoadActionLoad;
		m_mainAdditionalRenderPass.colorAttachments[0].storeAction = MTLStoreActionStore;
		
		m_mainAdditionalRenderPass.depthAttachment.texture = m_mainDepthBuffer;
		m_mainAdditionalRenderPass.depthAttachment.loadAction = MTLLoadActionClear;
		m_mainAdditionalRenderPass.depthAttachment.clearDepth = 1.0f;
		m_mainAdditionalRenderPass.depthAttachment.storeAction = MTLStoreActionDontCare;
	}
}

id<MTLSamplerState> CMetalGraphicDevice::GetSamplerState(const SAMPLER_STATE_INFO& stateInfo)
{
	id<MTLSamplerState> state = nil;
	
	uint32 stateKey = *reinterpret_cast<const uint32*>(&stateInfo);
	auto stateIterator = m_samplerStates.find(stateKey);
	if(stateIterator == std::end(m_samplerStates))
	{
		MTLSamplerDescriptor* stateDescriptor = [[MTLSamplerDescriptor alloc] init];
		stateDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
		stateDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
		
		state = [m_metalView.device newSamplerStateWithDescriptor: stateDescriptor];
		assert(state != nil);
		
		[stateDescriptor release];
		m_samplerStates.insert(std::make_pair(stateKey, state));
	}
	else
	{
		state = stateIterator->second;
	}
	
	return state;
}

void CMetalGraphicDevice::Draw()
{
	@autoreleasepool
	{
		id<CAMetalDrawable> drawable = nil;
		while(drawable == nil)
		{
			drawable = [m_metalView.renderLayer nextDrawable];
		}
		SetupMainRenderPass(drawable);
		
		id<MTLCommandBuffer> commandBuffer = [m_commandQueue commandBuffer];
		unsigned int constantBufferOffset = 0;
		
		//Draw shadow map(s)
		{
			id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor: m_shadowRenderPass];

			for(const auto& viewport : m_viewports)
			{
				if(!viewport->GetShadowCamera()) continue;
				DrawViewportShadowMap(renderEncoder, viewport, constantBufferOffset);
				break;
			}
			
			[renderEncoder endEncoding];
		}

		//Draw main maps
		{
			MTLRenderPassDescriptor* currentRenderPass = m_mainRenderPass;
			for(const auto& viewport : m_viewports)
			{
				id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor: currentRenderPass];
				DrawViewportMainMap(renderEncoder, viewport, constantBufferOffset);
				[renderEncoder endEncoding];
				currentRenderPass = m_mainAdditionalRenderPass;
			}
		}
		
		__block dispatch_semaphore_t block_sema = m_drawSemaphore;
		[commandBuffer addCompletedHandler:
			^(id<MTLCommandBuffer> buffer)
			{
				dispatch_semaphore_signal(block_sema);
			}
		];
		
		[commandBuffer presentDrawable: drawable];
		[commandBuffer commit];
				
		dispatch_semaphore_wait(m_drawSemaphore, DISPATCH_TIME_FOREVER);
	}
}

void CMetalGraphicDevice::DrawViewportMainMap(id<MTLRenderCommandEncoder> renderEncoder, CViewport* viewport, unsigned int& constantBufferOffset)
{
	auto camera = viewport->GetCamera();
	assert(camera);
	//auto cameraFrustum = camera->GetFrustum();
	auto shadowCamera = viewport->GetShadowCamera();
	
	std::vector<CMesh*> renderQueue;
	renderQueue.reserve(100);
	
	auto sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&] (const SceneNodePtr& node)
		{
			if(!node->GetVisible()) return false;
			
			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(mesh->GetPrimitiveCount() != 0)
				{
					renderQueue.push_back(mesh.get());
				}
			}
			else if(auto meshProvider = std::dynamic_pointer_cast<CMeshProvider>(node))
			{
				meshProvider->GetMeshes(renderQueue, camera.get());
			}
			return true;
		}
	);
	
	bool hasShadowMap = shadowCamera != nullptr;
	
	METALVIEWPORT_PARAMS viewportParams;
	viewportParams.viewport = viewport;
	viewportParams.projMatrix = camera->GetProjectionMatrix();
	viewportParams.viewMatrix = camera->GetViewMatrix();
	viewportParams.shadowViewProjMatrix = shadowCamera ? (shadowCamera->GetViewMatrix() * shadowCamera->GetProjectionMatrix()) : CMatrix4::MakeIdentity();
	viewportParams.hasShadowMap = hasShadowMap;
	
	for(const auto& mesh : renderQueue)
	{
		auto effectProvider = mesh->GetEffectProvider();
		auto effect = std::static_pointer_cast<CMetalEffect>(effectProvider->GetEffectForRenderable(mesh, hasShadowMap));
		unsigned int constantsSize = effect->GetConstantsSize();
		assert((constantBufferOffset + constantsSize) <= CONSTANT_BUFFER_SIZE);
		DrawMesh(renderEncoder, constantBufferOffset, viewportParams, mesh, effect);
		constantBufferOffset += constantsSize;
	}
}

void CMetalGraphicDevice::DrawViewportShadowMap(id<MTLRenderCommandEncoder> renderEncoder, CViewport* viewport, unsigned int& constantBufferOffset)
{
	auto camera = viewport->GetShadowCamera();
	assert(camera);

	std::vector<CMesh*> renderQueue;
	renderQueue.reserve(100);
	
	auto sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&] (const SceneNodePtr& node)
		{
			if(!node->GetVisible()) return false;
			
			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(mesh->GetMaterial()->GetShadowCasting())
				{
					renderQueue.push_back(mesh.get());
				}
			}
			return true;
		}
	);
	
	METALVIEWPORT_PARAMS viewportParams;
	viewportParams.viewport = viewport;
	viewportParams.projMatrix = camera->GetProjectionMatrix();
	viewportParams.viewMatrix = camera->GetViewMatrix();
	
	for(const auto& mesh : renderQueue)
	{
		auto effect = std::static_pointer_cast<CMetalEffect>(m_shadowMapEffectProvider->GetEffectForRenderable(mesh, false));
		unsigned int constantsSize = effect->GetConstantsSize();
		assert((constantBufferOffset + constantsSize) <= CONSTANT_BUFFER_SIZE);
		DrawMesh(renderEncoder, constantBufferOffset, viewportParams, mesh, effect);
		constantBufferOffset += constantsSize;
	}
}

void CMetalGraphicDevice::DrawMesh(id<MTLRenderCommandEncoder> renderEncoder, unsigned int constantBufferOffset, const METALVIEWPORT_PARAMS& viewportParams, CMesh* mesh, const MetalEffectPtr& effect)
{
	auto vertexBuffer = std::static_pointer_cast<CMetalVertexBuffer>(mesh->GetVertexBuffer());
	assert(vertexBuffer);
	
	auto material = mesh->GetMaterial();
	assert(material);
	
	auto vertexBufferHandle = vertexBuffer->GetVertexBufferHandle();
	auto indexBufferHandle = vertexBuffer->GetIndexBufferHandle();
		
	effect->UpdateConstants(reinterpret_cast<uint8*>(m_constantBuffer.contents) + constantBufferOffset, viewportParams, material.get(), mesh->GetWorldTransformation());
	
	CMetalEffect::PIPELINE_STATE_INFO pipelineStateInfo = {};
	pipelineStateInfo.blendingMode = material->GetAlphaBlendingMode();
	id<MTLRenderPipelineState> pipelineState = effect->GetPipelineState(pipelineStateInfo);
	
	for(unsigned int i = 0; i < CMaterial::MAX_TEXTURE_SLOTS; i++)
	{
		auto texture = std::static_pointer_cast<CMetalTexture>(material->GetTexture(i));
		if(texture)
		{
			SAMPLER_STATE_INFO samplerStateInfo = {};
			id<MTLSamplerState> samplerState = GetSamplerState(samplerStateInfo);
			[renderEncoder setFragmentSamplerState: samplerState atIndex: i];
			
			id<MTLTexture> textureHandle = reinterpret_cast<id<MTLTexture>>(texture->GetHandle());
			[renderEncoder setFragmentTexture: textureHandle atIndex: i];
		}
	}
	if(viewportParams.hasShadowMap)
	{
		[renderEncoder setFragmentTexture: m_shadowMap atIndex: 1];
	}
	
	MTLPrimitiveType primitiveType = MTLPrimitiveTypeTriangle;
	unsigned int indexCount = vertexBuffer->GetDescriptor().indexCount;
	switch(mesh->GetPrimitiveType())
	{
		case PRIMITIVE_TRIANGLE_STRIP:
			primitiveType = MTLPrimitiveTypeTriangleStrip;
			break;
		case PRIMITIVE_TRIANGLE_LIST:
			primitiveType = MTLPrimitiveTypeTriangle;
			break;
		case PRIMITIVE_LINE_LIST:
			primitiveType = MTLPrimitiveTypeLine;
			break;
		default:
			assert(0);
			break;
	}
	
	[renderEncoder setRenderPipelineState: pipelineState];
	[renderEncoder setDepthStencilState: m_depthStencilState];
	[renderEncoder setVertexBuffer: vertexBufferHandle offset: 0 atIndex: 0];
	[renderEncoder setVertexBuffer: m_constantBuffer offset: constantBufferOffset atIndex: 1];
	[renderEncoder drawIndexedPrimitives: primitiveType indexCount: indexCount indexType: MTLIndexTypeUInt16 indexBuffer: indexBufferHandle indexBufferOffset: 0];
}
