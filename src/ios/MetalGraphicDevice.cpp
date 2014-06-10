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
	m_constantBuffer = [m_metalView.device newBufferWithLength: CONSTANT_BUFFER_SIZE options: MTLResourceOptionCPUCacheModeDefault];
	m_defaultEffectProvider = std::make_shared<CMetalUberEffectProvider>(m_metalView.device);
	m_drawSemaphore = dispatch_semaphore_create(0);
}

CMetalGraphicDevice::~CMetalGraphicDevice()
{
	[m_constantBuffer release];
	[m_commandQueue release];
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

void CMetalGraphicDevice::Draw()
{
	@autoreleasepool
	{
		id<CAMetalDrawable> drawable = nil;
		while(drawable == nil)
		{
			drawable = [m_metalView.renderLayer newDrawable];
		}

		MTLAttachmentDescriptor* colorAttachment = [MTLAttachmentDescriptor attachmentDescriptorWithTexture: drawable.texture];
		[colorAttachment setLoadAction: MTLLoadActionClear];
		[colorAttachment setClearValue: MTLClearValueMakeColor(0.0f, 0.0f, 0.0f, 0.0f)];
		[colorAttachment setStoreAction: MTLStoreActionStore];
		
		MTLFramebufferDescriptor* framebufferDescriptor = [MTLFramebufferDescriptor framebufferDescriptorWithColorAttachment: colorAttachment];
		
		id<MTLFramebuffer> framebuffer = [m_metalView.device newFramebufferWithDescriptor: framebufferDescriptor];
		
		id<MTLCommandBuffer> commandBuffer = [m_commandQueue commandBuffer];
		id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithFramebuffer: framebuffer];
		
		unsigned int constantBufferOffset = 0;
		for(const auto& viewport : m_viewports)
		{
			DrawViewport(renderEncoder, viewport, constantBufferOffset);
		}
		
		[renderEncoder endEncoding];
		
		__block dispatch_semaphore_t block_sema = m_drawSemaphore;
		[commandBuffer addCompletedHandler:
			^(id<MTLCommandBuffer> buffer)
			{
				dispatch_semaphore_signal(block_sema);
			}
		];
		
		[commandBuffer addScheduledPresent: drawable];
		[commandBuffer commit];
		
		[framebuffer release];
		[drawable release];
		
		dispatch_semaphore_wait(m_drawSemaphore, DISPATCH_TIME_FOREVER);
	}
}

void CMetalGraphicDevice::DrawViewport(id<MTLRenderCommandEncoder> renderEncoder, CViewport* viewport, unsigned int& constantBufferOffset)
{
	DrawViewportMainMap(renderEncoder, viewport, constantBufferOffset);
}

void CMetalGraphicDevice::DrawViewportMainMap(id<MTLRenderCommandEncoder> renderEncoder, CViewport* viewport, unsigned int& constantBufferOffset)
{
	auto camera = viewport->GetCamera();
	assert(camera);
	//auto cameraFrustum = camera->GetFrustum();

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
	
	METALVIEWPORT_PARAMS viewportParams;
	viewportParams.viewport = viewport;
	viewportParams.projMatrix = camera->GetProjectionMatrix();
	viewportParams.viewMatrix = camera->GetViewMatrix();
	
	for(const auto& mesh : renderQueue)
	{
		auto effectProvider = mesh->GetEffectProvider();
		auto effect = std::static_pointer_cast<CMetalEffect>(effectProvider->GetEffectForRenderable(mesh, false));
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
	
	auto vertexBufferHandle = vertexBuffer->GetVertexBufferHandle();
	auto indexBufferHandle = vertexBuffer->GetIndexBufferHandle();
	
	auto vertexShaderHandle = effect->GetVertexShaderHandle();
	auto fragmentShaderHandle = effect->GetFragmentShaderHandle();
	
	effect->UpdateConstants(reinterpret_cast<uint8*>(m_constantBuffer.contents) + constantBufferOffset, viewportParams, mesh->GetMaterial().get(), mesh->GetWorldTransformation());
	
	MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
	[pipelineStateDescriptor setPixelFormat: MTLPixelFormatBGRA8Unorm atIndex: MTLFramebufferAttachmentIndexColor0];
	[pipelineStateDescriptor setSampleCount: 1];
	[pipelineStateDescriptor setVertexFunction: vertexShaderHandle];
	[pipelineStateDescriptor setFragmentFunction: fragmentShaderHandle];
	
	NSError* pipelineStateError = nil;
	id<MTLRenderPipelineState> pipelineState = [m_metalView.device newRenderPipelineStateWithDescriptor: pipelineStateDescriptor error: &pipelineStateError];
	assert(pipelineStateError == nil);
	
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
	[renderEncoder setVertexBuffer: vertexBufferHandle offset: 0 atIndex: 0];
	[renderEncoder setVertexBuffer: m_constantBuffer offset: constantBufferOffset atIndex: 1];
	[renderEncoder drawIndexedPrimitives: primitiveType indexCount: indexCount indexType: MTLIndexTypeUInt16 indexBuffer: indexBufferHandle indexBufferOffset: 0];
	
	[pipelineStateDescriptor release];
	[pipelineState release];
}
