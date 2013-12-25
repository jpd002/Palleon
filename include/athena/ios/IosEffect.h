#pragma once

#include "athena/Effect.h"
#include "athena/Matrix4.h"
#include "athena/Material.h"
#include "athena/Ios/OpenGlEsDefs.h"

namespace Athena
{
	class CIosEffect : public CEffect
	{
	public:
						CIosEffect();
		virtual			~CIosEffect();
		
		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&) = 0;
		
		GLuint			GetProgram() const;
				
	protected:
		void			BuildProgram(const std::string&, const std::string&);
		
		GLuint			m_program = -1;
		
	private:
		GLuint			CompileShader(const char*, GLenum);
		void			DumpProgramLog(GLuint);
	};
}
