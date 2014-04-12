#pragma once

#include <vector>
#include "athena/Effect.h"
#include "athena/Matrix4.h"
#include "athena/Material.h"
#include "athena/gles/OpenGlEsDefs.h"

namespace Athena
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
