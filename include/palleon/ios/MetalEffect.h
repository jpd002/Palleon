#pragma once

#include <Metal/Metal.h>
#include "Types.h"
#include <string>
#include <map>
#include "palleon/Matrix4.h"
#include "palleon/Effect.h"

namespace Palleon
{
	class CViewport;
	class CMaterial;
	
	struct METALVIEWPORT_PARAMS
	{
		CViewport*	viewport = nullptr;
		CMatrix4	viewMatrix;
		CMatrix4	projMatrix;
	};
	
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
		
		virtual void				UpdateConstants(void*, const METALVIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) = 0;
		virtual unsigned int		GetConstantsSize() const = 0;
		
	protected:
		typedef std::map<uint32, id<MTLRenderPipelineState>> PipelineStateMap;
		
		void						CreateLibraryAndShaders(const std::string&);
		
		id<MTLDevice>				m_device = nil;
		id<MTLLibrary>				m_library = nil;
		id<MTLFunction>				m_vertexShader = nil;
		id<MTLFunction>				m_fragmentShader = nil;
		PipelineStateMap			m_pipelineStates;
	};
	
	typedef std::shared_ptr<CMetalEffect> MetalEffectPtr;
}
