#include "palleon/gles/GlEsEffect.h"
#include "palleon/VertexBuffer.h"

using namespace Palleon;

CGlEsEffect::CGlEsEffect()
{
	
}

CGlEsEffect::~CGlEsEffect()
{
	if(m_program != -1)
	{
		glDeleteProgram(m_program);		
	}
}

GLuint CGlEsEffect::GetProgram() const
{
	return m_program;
}

void CGlEsEffect::ValidateProgram()
{
	glValidateProgram(m_program);
	DumpProgramLog(m_program);
	
	{
		GLint status = 0;
		glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);
		assert(status != 0);
	}
}

void CGlEsEffect::BuildProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource, const AttributeBindingArray& attributeBindings)
{	
	GLuint vertexShader = CompileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
	GLuint pixelShader = CompileShader(pixelShaderSource.c_str(), GL_FRAGMENT_SHADER);
	
	assert(vertexShader != -1);
	assert(pixelShader != -1);
	
	GLuint program = glCreateProgram();
	
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);

	for(const auto& attributeBinding : attributeBindings)
	{
		glBindAttribLocation(program, attributeBinding.first, attributeBinding.second.c_str());
	}
	
	glLinkProgram(program);
	DumpProgramLog(program);
	
	{
		GLint status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		assert(status != 0);
	}
		
	glDeleteShader(vertexShader);
	glDeleteShader(pixelShader);
	
	CHECKGLERROR();
	
	m_program = program;
}

GLuint CGlEsEffect::CompileShader(const char* shaderSource, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	
#if defined(_DEBUG)
	GLint logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar *log = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		printf("Failed to compile shader:\n%s", shaderSource);
		printf("Shader compile log:\n%s", log);
		delete [] log;
	}
#endif
	
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		glDeleteShader(shader);
		return -1;
	}
	
	return shader;
}

void CGlEsEffect::DumpProgramLog(GLuint program)
{
#if defined(_DEBUG)
	GLint logLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		GLchar *log = new GLchar[logLength];
		glGetProgramInfoLog(program, logLength, &logLength, log);
		printf("Program link log:\n%s", log);
		delete [] log;
	}
#endif
}
