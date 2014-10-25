#pragma once

#include <Metal/Metal.h>
#include "Types.h"
#include <string>
#include <map>
#include "palleon/Matrix4.h"
#include "palleon/Effect.h"

namespace Palleon
{
	class CMetalEffect : public CEffect
	{
	public:
		struct PIPELINE_STATE_INFO
		{
			unsigned int	blendingMode : 2;
			unsigned int	unused : 30;
		};
									CMetalEffect(id<MTLDevice>);
		virtual						~CMetalEffect();
		
		id<MTLFunction>				GetVertexShaderHandle() const;
		id<MTLFunction>				GetFragmentShaderHandle() const;
		id<MTLRenderPipelineState>	GetPipelineState(const PIPELINE_STATE_INFO&);
		
		virtual unsigned int		GetConstantsSize() const = 0;

		void						SetConstantBuffer(uint8*);
		
	protected:
		struct OffsetKeeper
		{
			uint32 Allocate(uint32 size)
			{
				uint32 result = currentOffset;
				currentOffset += size;
				return result;
			}
			
			uint32 currentOffset = 0;
		};
		
		typedef std::map<uint32, id<MTLRenderPipelineState>> PipelineStateMap;
		
		void						CreateLibraryAndShaders(const std::string&);
		
		virtual void				FillPipelinePixelFormats(MTLRenderPipelineDescriptor*) = 0;
		
		id<MTLDevice>				m_device = nil;
		id<MTLLibrary>				m_library = nil;
		id<MTLFunction>				m_vertexShader = nil;
		id<MTLFunction>				m_fragmentShader = nil;
		PipelineStateMap			m_pipelineStates;
		uint8*						m_constantBuffer = nullptr;
	};
	
	typedef std::shared_ptr<CMetalEffect> MetalEffectPtr;
}
