#pragma once

#include <Metal/Metal.h>
#include <string>
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
								CMetalEffect(id<MTLDevice>);
		virtual					~CMetalEffect();
		
		id<MTLFunction>			GetVertexShaderHandle() const;
		id<MTLFunction>			GetFragmentShaderHandle() const;
		
		virtual void			UpdateConstants(void*, const METALVIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) = 0;
		virtual unsigned int	GetConstantsSize() const = 0;
		
	protected:
		void					CreateLibraryAndShaders(const std::string&);
		
		id<MTLDevice>			m_device;
		id<MTLLibrary>			m_library;
		id<MTLFunction>			m_vertexShader;
		id<MTLFunction>			m_fragmentShader;
	};
	
	typedef std::shared_ptr<CMetalEffect> MetalEffectPtr;
}
