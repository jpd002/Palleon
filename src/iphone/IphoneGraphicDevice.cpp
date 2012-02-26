#include "IphoneGraphicDevice.h"
#include "athena/iphone/IphoneVertexBuffer.h"
#include "athena/iphone/IphoneTexture.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"

using namespace Athena;

static const GLenum g_textureAddressModes[TEXTURE_ADDRESS_MODE_MAX] =
{
	GL_CLAMP_TO_EDGE,
	GL_REPEAT,
};

CIphoneGraphicDevice::CIphoneGraphicDevice(bool hasRetinaDisplay, const CVector2& screenSize)
: m_hasRetinaDisplay(hasRetinaDisplay)
{
	m_screenSize = screenSize;
	m_renderQueue.reserve(0x10000);
}

CIphoneGraphicDevice::~CIphoneGraphicDevice()
{

}

void CIphoneGraphicDevice::CreateInstance(bool hasRetinaDisplay, const CVector2& screenSize)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CIphoneGraphicDevice(hasRetinaDisplay, screenSize);
}

void CIphoneGraphicDevice::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}

void CIphoneGraphicDevice::Draw()
{
	if(m_hasRetinaDisplay)
	{
		glViewport(0, 0, m_screenSize.x * 2, m_screenSize.y * 2);
		glScissor(0, 0, m_screenSize.x * 2, m_screenSize.y * 2);
	}
	else
	{
		glViewport(0, 0, m_screenSize.x, m_screenSize.y);
		glScissor(0, 0, m_screenSize.x, m_screenSize.y);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//Reset metrics
	m_drawCallCount = 0;
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDisable(GL_CULL_FACE);

	//Draw all viewports
	for(CViewport* viewport : m_viewports)
	{
		DrawViewport(viewport);
	}
	
	CHECKGLERROR();	
}

void CIphoneGraphicDevice::DrawViewport(CViewport* viewport)
{
	glClearDepthf(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	CHECKGLERROR();
	
	CameraPtr camera = viewport->GetCamera();
	
	m_viewProjMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	
	m_renderQueue.clear();
	
	const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(std::bind(&CIphoneGraphicDevice::FillRenderQueue, this, std::placeholders::_1, camera.get()));
	
	for(CMesh* mesh : m_renderQueue)
	{
		DrawMesh(mesh);
	}
}

VertexBufferPtr CIphoneGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& bufferDesc)
{
	return VertexBufferPtr(new CIphoneVertexBuffer(bufferDesc));
}

TexturePtr CIphoneGraphicDevice::CreateTextureFromFile(const char* path)
{
	return TexturePtr(new CIphoneTexture(path));
}

TexturePtr CIphoneGraphicDevice::CreateTextureFromMemory(const void* data, uint32 size)
{
	return TexturePtr(new CIphoneTexture(data, size));
}

TexturePtr CIphoneGraphicDevice::CreateTextureFromRawData(const void* data, TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return TexturePtr(new CIphoneTexture(data, textureFormat, width, height));
}

TexturePtr CIphoneGraphicDevice::CreateCubeTextureFromFile(const char* path)
{
	return TexturePtr();
}

RenderTargetPtr CIphoneGraphicDevice::CreateRenderTarget(TEXTURE_FORMAT textureFormat, uint32 width, uint32 height)
{
	return RenderTargetPtr();
}

CubeRenderTargetPtr CIphoneGraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT textureFormat, uint32 size)
{
	return CubeRenderTargetPtr();
}

bool CIphoneGraphicDevice::FillRenderQueue(CSceneNode* node, CCamera* camera)
{
	if(!node->GetVisible()) return false;
	
	if(CMesh* mesh = dynamic_cast<CMesh*>(node))
	{
		m_renderQueue.push_back(mesh);
	}
	else if(CMeshProvider* meshProvider = dynamic_cast<CMeshProvider*>(node))
	{
		meshProvider->GetMeshes(m_renderQueue, camera);
	}
	return true;
}

void CIphoneGraphicDevice::DrawMesh(CMesh* mesh)
{
	if(mesh->GetPrimitiveCount() == 0) return;
	
	CIphoneVertexBuffer* vertexBufferGen = static_cast<CIphoneVertexBuffer*>(mesh->GetVertexBuffer().get());
	assert(vertexBufferGen != NULL);
	
	const VERTEX_BUFFER_DESCRIPTOR& descriptor = vertexBufferGen->GetDescriptor();
	GLuint vertexBuffer = vertexBufferGen->GetVertexBuffer();
	uint16* indexBuffer = vertexBufferGen->GetIndexBuffer();
	GLuint vertexArray = vertexBufferGen->GetVertexArray();
	
	CVector3 worldPosition = mesh->GetWorldPosition();
	CVector3 worldScale = mesh->GetWorldScale();
	
	CMatrix4 worldMatrix = CMatrix4::MakeTranslation(worldPosition.x, worldPosition.y, worldPosition.z);
	worldMatrix(0, 0) = worldScale.x;
	worldMatrix(1, 1) = worldScale.y;
	worldMatrix(2, 2) = worldScale.z;
	
	const EFFECTINFO* currentEffect(NULL);
	
	//Setup material
	{
		auto material = mesh->GetMaterial();
		assert(material);
		CColor meshColor = material->GetColor();
		
		CIphoneEffectGenerator::EFFECTCAPS effectCaps;
		memset(&effectCaps, 0, sizeof(effectCaps));
		
		if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_COLOR)
		{
			effectCaps.hasVertexColor = true;
		}
		
		for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
		{
			if(material->GetTexture(i))
			{
				effectCaps.setHasDiffuseMap(i, true);
				effectCaps.setDiffuseMapCoordSrc(i, material->GetTextureCoordSource(i));
				if(i != 0)
				{
					unsigned int combineMode = DIFFUSE_MAP_COMBINE_MODULATE;
					switch(material->GetTextureCombineMode(i))
					{
						case TEXTURE_COMBINE_MODULATE:
							combineMode = DIFFUSE_MAP_COMBINE_MODULATE;
							break;
					}
					effectCaps.setDiffuseMapCombineMode(i, combineMode);
				}
			}
		}
		
		uint32 effectKey = *reinterpret_cast<uint32*>(&effectCaps);
		auto effectIterator = m_effects.find(effectKey);
		if(effectIterator == std::end(m_effects))
		{
			GenerateEffect(effectCaps);
			effectIterator = m_effects.find(effectKey);
		}
		
		currentEffect = &effectIterator->second;
		
		glUseProgram(currentEffect->program);
		CHECKGLERROR();
		
		glUniformMatrix4fv(currentEffect->viewProjMatrixHandle, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&m_viewProjMatrix));
		glUniformMatrix4fv(currentEffect->worldMatrixHandle, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&worldMatrix));
		glUniform4f(currentEffect->meshColorHandle, meshColor.r, meshColor.g, meshColor.b, meshColor.a);
		
		for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
		{
			if(currentEffect->diffuseTexture[i] != -1)
			{
				glUniform1i(currentEffect->diffuseTexture[i], i);
				GLuint textureHandle = reinterpret_cast<GLuint>(material->GetTexture(i)->GetHandle());
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, textureHandle);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, g_textureAddressModes[material->GetTextureAddressModeU(i)]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, g_textureAddressModes[material->GetTextureAddressModeV(i)]);
				CHECKGLERROR();
			}
		}
		
		if(material->GetIsTransparent())
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		CHECKGLERROR();		
	}
		
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	CHECKGLERROR();
	
	glBindVertexArrayOES(vertexArray);
	CHECKGLERROR();
	
	GLenum primitiveType = GL_TRIANGLES;
	GLsizei primitiveCount = mesh->GetPrimitiveCount();
	GLsizei vertexCount = primitiveCount * 3;
	switch(mesh->GetPrimitiveType())
	{
		case PRIMITIVE_TRIANGLE_STRIP:
			primitiveType = GL_TRIANGLE_STRIP;
			vertexCount = (primitiveCount + 2);
			break;
		case PRIMITIVE_TRIANGLE_LIST:
			primitiveType = GL_TRIANGLES;
			vertexCount = (primitiveCount * 3);
			break;
	}
	
	glDrawElements(primitiveType, vertexCount, GL_UNSIGNED_SHORT, indexBuffer);
	CHECKGLERROR();
	
	m_drawCallCount++;
}

void CIphoneGraphicDevice::GenerateEffect(const CIphoneEffectGenerator::EFFECTCAPS& effectCaps)
{
	EFFECTINFO newEffect;
	
	newEffect.program = BuildProgram(effectCaps);
	
	newEffect.viewProjMatrixHandle	= glGetUniformLocation(newEffect.program, "c_viewProjMatrix");
	newEffect.worldMatrixHandle		= glGetUniformLocation(newEffect.program, "c_worldMatrix");
	newEffect.meshColorHandle		= glGetUniformLocation(newEffect.program, "c_meshColor");
	
	for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
	{
		{
			char paramName[256];
			sprintf(paramName, "c_diffuseTexture%d", i);
			newEffect.diffuseTexture[i]	= glGetUniformLocation(newEffect.program, paramName);			
		}
	}
	
	uint32 effectKey = *reinterpret_cast<const uint32*>(&effectCaps);
	m_effects[effectKey] = newEffect;
}

GLuint CIphoneGraphicDevice::CompileShader(const char* shaderSource, GLenum shaderType)
{
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	
#if defined(DEBUG)
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

void CIphoneGraphicDevice::DumpProgramLog(GLuint program)
{
#if defined(DEBUG)
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

GLuint CIphoneGraphicDevice::BuildProgram(const CIphoneEffectGenerator::EFFECTCAPS& effectCaps)
{
	std::string vertexShaderSource = CIphoneEffectGenerator::GenerateVertexShader(effectCaps);
	std::string pixelShaderSource = CIphoneEffectGenerator::GeneratePixelShader(effectCaps);
	
	GLuint vertexShader = CompileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
	GLuint pixelShader = CompileShader(pixelShaderSource.c_str(), GL_FRAGMENT_SHADER);
	
	assert(vertexShader != -1);
	assert(pixelShader != -1);
	
	GLuint program = glCreateProgram();
	
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);
	
	glBindAttribLocation(program, VERTEX_ATTRIB_POSITION, "a_position");
	glBindAttribLocation(program, VERTEX_ATTRIB_TEXCOORD0, "a_texCoord0");
	glBindAttribLocation(program, VERTEX_ATTRIB_COLOR, "a_color");
	
	glLinkProgram(program);
	DumpProgramLog(program);
	
	{
		GLint status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		assert(status != 0);
	}
	
	glValidateProgram(program);
	DumpProgramLog(program);
	
	{
		GLint status = 0;
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
		assert(status != 0);
	}
		
	glDeleteShader(vertexShader);
	glDeleteShader(pixelShader);
	
	CHECKGLERROR();
	
	return program;
}

void CIphoneGraphicDevice::SetFrameRate(float frameRate)
{
	m_frameRate = frameRate;
}

uint32 CGraphicDevice::ConvertColorToUInt32(const CColor& color)
{
	return 
		(static_cast<uint32>(color.a * 255.f) << 24) |
		(static_cast<uint32>(color.r * 255.f) <<  0) |
		(static_cast<uint32>(color.g * 255.f) <<  8) |
		(static_cast<uint32>(color.b * 255.f) << 16);
}
