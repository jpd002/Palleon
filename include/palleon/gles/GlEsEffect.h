#pragma once

#include <vector>
#include "palleon/Effect.h"
#include "palleon/Matrix4.h"
#include "palleon/Material.h"
#include "palleon/gles/OpenGlEsDefs.h"

namespace Palleon
{
	class CGlEsEffect : public CEffect
	{
	public:
						CGlEsEffect();
		virtual			~CGlEsEffect();
		
		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) = 0;
		
		GLuint			GetProgram() const;
				
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
