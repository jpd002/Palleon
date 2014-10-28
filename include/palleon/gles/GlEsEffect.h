#pragma once

#include <vector>
#include "palleon/graphics/Effect.h"
#include "palleon/gles/OpenGlEsDefs.h"

namespace Palleon
{
	class CViewport;
	
	class CGlEsEffect : public CEffect
	{
	public:
						CGlEsEffect();
		virtual			~CGlEsEffect();
		
		GLuint			GetProgram() const;
		void			ValidateProgram();
		
	protected:
		typedef std::pair<uint32, std::string> AttributeBinding;
		typedef std::vector<AttributeBinding> AttributeBindingArray;
		
		void			BuildProgram(const std::string&, const std::string&, const AttributeBindingArray&);
		
		GLuint			m_program = -1;
		
	private:
		GLuint			CompileShader(const char*, GLenum);
		void			DumpProgramLog(GLuint);
	};
	
	typedef std::shared_ptr<CGlEsEffect> GlEsEffectPtr;
}
