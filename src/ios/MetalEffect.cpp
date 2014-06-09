#include <cassert>
#include "palleon/ios/MetalEffect.h"

using namespace Palleon;

CMetalEffect::CMetalEffect(id<MTLDevice> device)
: m_device(device)
{

}

CMetalEffect::~CMetalEffect()
{
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
