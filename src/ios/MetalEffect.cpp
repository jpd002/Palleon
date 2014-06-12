#include <cassert>
#include "palleon/Material.h"
#include "palleon/ios/MetalEffect.h"

using namespace Palleon;

CMetalEffect::CMetalEffect(id<MTLDevice> device)
: m_device(device)
{

}

CMetalEffect::~CMetalEffect()
{
	assert(m_pipelineStates.empty());
	[m_vertexShader release];
	[m_fragmentShader release];
	[m_library release];
}

id<MTLFunction> CMetalEffect::GetVertexShaderHandle() const
{
	return m_vertexShader;
}

id<MTLFunction> CMetalEffect::GetFragmentShaderHandle() const
{
	return m_fragmentShader;
}

id<MTLRenderPipelineState> CMetalEffect::GetPipelineState(const PIPELINE_STATE_INFO& stateInfo)
{
	id<MTLRenderPipelineState> state = nil;
	
	uint32 stateKey = *reinterpret_cast<const uint32*>(&stateInfo);
	auto stateIterator = m_pipelineStates.find(stateKey);
	if(stateIterator == std::end(m_pipelineStates))
	{
		assert(m_vertexShader != nil);
		assert(m_fragmentShader != nil);
	
		MTLRenderPipelineDescriptor* pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
		[pipelineStateDescriptor setPixelFormat: MTLPixelFormatBGRA8Unorm atIndex: MTLFramebufferAttachmentIndexColor0];
		[pipelineStateDescriptor setSampleCount: 1];
		[pipelineStateDescriptor setVertexFunction: m_vertexShader];
		[pipelineStateDescriptor setFragmentFunction: m_fragmentShader];

		if(stateInfo.blendingMode != ALPHA_BLENDING_NONE)
		{
			MTLBlendDescriptor* blendDescriptor = [[MTLBlendDescriptor alloc] init];
			blendDescriptor.blendingEnabled = TRUE;
			switch(stateInfo.blendingMode)
			{
			case ALPHA_BLENDING_LERP:
				blendDescriptor.alphaBlendOperation = MTLBlendOperationAdd;
				blendDescriptor.sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
				blendDescriptor.destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
				break;
			default:
				assert(0);
				break;
			}
			[pipelineStateDescriptor setBlendDescriptor: blendDescriptor atIndex: MTLFramebufferAttachmentIndexColor0];
			[blendDescriptor release];
		}
	
		NSError* pipelineStateError = nil;
		state = [m_device newRenderPipelineStateWithDescriptor: pipelineStateDescriptor error: &pipelineStateError];
		assert(pipelineStateError == nil);
		
		[pipelineStateDescriptor release];
		
		m_pipelineStates.insert(std::make_pair(stateKey, state));
	}
	else
	{
		state = stateIterator->second;
	}
	
	return state;
}

void CMetalEffect::CreateLibraryAndShaders(const std::string& librarySource)
{
	NSError* compileError = nil;

	m_library = [m_device
		newLibraryWithSource: [NSString stringWithUTF8String: librarySource.c_str()]
		options: nil
		error: &compileError
	];
	
	if(compileError)
	{
		NSLog(@"%s", librarySource.c_str());
		NSLog(@"%@", [compileError localizedDescription]);
		assert(false);
	}
	
	assert(m_library != nil);
	
	m_vertexShader = [m_library newFunctionWithName: @"VertexShader"];
	m_fragmentShader = [m_library newFunctionWithName: @"FragmentShader"];
	
	assert(m_vertexShader != nil && m_fragmentShader != nil);
}
