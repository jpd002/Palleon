#include "IphoneGraphicDevice.h"
#include "athena/iphone/IphoneVertexBuffer.h"
#include "athena/iphone/IphoneTexture.h"
#include "athena/Mesh.h"
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

using namespace Athena;

CIphoneGraphicDevice::CIphoneGraphicDevice(bool hasRetinaDisplay, const CVector2& screenSize)
: m_hasRetinaDisplay(hasRetinaDisplay)
{
	m_screenSize = screenSize;
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
    //    glDisable(GL_CULL_FACE);

	//Draw all viewports
	for(ViewportList::iterator viewportIterator(m_viewports.begin());
		viewportIterator != m_viewports.end(); viewportIterator++)
	{
        glClearDepthf(1.0f);
        glClear(GL_DEPTH_BUFFER_BIT);
        
		CViewport* viewport(*viewportIterator);
		CameraPtr camera = viewport->GetCamera();

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(reinterpret_cast<const float*>(&camera->GetProjectionMatrix()));
        assert(glGetError() == GL_NO_ERROR);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(reinterpret_cast<const float*>(&camera->GetViewMatrix()));
        assert(glGetError() == GL_NO_ERROR);
        
        const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
        sceneRoot->TraverseNodes(std::tr1::bind(&CIphoneGraphicDevice::DrawNode, this, std::tr1::placeholders::_1));
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

TexturePtr CIphoneGraphicDevice::CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32)
{
    return TexturePtr();
}

bool CIphoneGraphicDevice::DrawNode(CSceneNode* node)
{
	if(CMesh* mesh = dynamic_cast<CMesh*>(node))
	{
		if(mesh->GetPrimitiveCount() == 0) return true;
        
		CIphoneVertexBuffer* vertexBufferGen = static_cast<CIphoneVertexBuffer*>(mesh->GetVertexBuffer().get());
		assert(vertexBufferGen != NULL);
        
		const VERTEX_BUFFER_DESCRIPTOR& descriptor = vertexBufferGen->GetDescriptor();
        GLuint vertexBuffer = vertexBufferGen->GetVertexBuffer();
        uint16* indexBuffer = vertexBufferGen->GetIndexBuffer();
        
		CVector2 worldPosition = node->GetWorldPosition();
		CVector2 worldScale = node->GetWorldScale();
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(worldPosition.x, worldPosition.y, 0);
        glScalef(worldScale.x, worldScale.y, 1);
        
		//Setup material
		{
			CColor color = mesh->GetColor();
			MaterialPtr material = mesh->GetMaterial();
			assert(material != NULL);
			RENDER_TYPE renderType = material->GetRenderType();
            
//			m_device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a));
            
//			m_device->SetTexture(0, NULL);
//			m_device->SetTexture(1, NULL);
            
			unsigned int currentStage = 0;
            
			if(renderType == RENDER_DIFFUSE || renderType == RENDER_LIGHTMAPPED)
			{
				TexturePtr texture = material->GetTexture(0);
                
				if(texture)
				{
                    CIphoneTexture* textureGen = static_cast<CIphoneTexture*>(texture.get());
                    
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, textureGen->GetTexture());
                    
					currentStage++;
				}
                
				//m_device->SetTexture(1, NULL);
			}
//			else if(material->GetRenderType() == RENDER_LIGHTMAPPED)
//			{
//				TexturePtr diffuseTexture = material->GetTexture(0);
//				TexturePtr lightMapTexture = material->GetTexture(1);
//                
//				if(diffuseTexture)
//				{
//					CDx9Texture* textureGen = static_cast<CDx9Texture*>(diffuseTexture.get());
//					m_device->SetTexture(0, textureGen->GetTexture());
//				}
//				else
//				{
//					m_device->SetTexture(0, NULL);
//				}			
                
//				m_device->SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
//				m_device->SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
//				m_device->SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_CURRENT);
                
//				m_device->SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
//				m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
//				m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);
                
//				if(lightMapTexture)
//				{
//					CDx9Texture* textureGen = static_cast<CDx9Texture*>(lightMapTexture.get());
//					m_device->SetTexture(1, textureGen->GetTexture());
//				}
//				else
//				{
//					m_device->SetTexture(1, NULL);
//				}
                
//				m_device->SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
//				m_device->SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
//				m_device->SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_CURRENT);
                
//				m_device->SetTextureStageState(1, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
//				m_device->SetTextureStageState(1, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
//				m_device->SetTextureStageState(1, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);
                
//				currentStage = 2;
//			}
            
			//Global coloring stage
			{
//				m_device->SetTextureStageState(currentStage, D3DTSS_COLOROP,	D3DTOP_MODULATE);
//				m_device->SetTextureStageState(currentStage, D3DTSS_COLORARG1,	D3DTA_TFACTOR);
//				m_device->SetTextureStageState(currentStage, D3DTSS_COLORARG2,	D3DTA_CURRENT);
                
//				m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
//				m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAARG1,	D3DTA_TFACTOR);
//				m_device->SetTextureStageState(currentStage, D3DTSS_ALPHAARG2,	D3DTA_CURRENT);
                
				currentStage++;
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
		}

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        assert(glGetError() == GL_NO_ERROR);
        
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        uint32 vertexSize = descriptor.GetVertexSize();
        
        if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_POS)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.posOffset));
        }
        else
        {
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        assert(glGetError() == GL_NO_ERROR);
        
        if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_UV0)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.uv0Offset));
        }
        else
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        assert(glGetError() == GL_NO_ERROR);
        
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
        assert(glGetError() == GL_NO_ERROR);
        
        glBindBuffer(GL_ARRAY_BUFFER, NULL);
        assert(glGetError() == GL_NO_ERROR);
        
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        
		m_drawCallCount++;
	}
	return true;
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
