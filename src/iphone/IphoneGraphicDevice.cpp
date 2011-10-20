#include "IphoneGraphicDevice.h"
#include "athena/iphone/IphoneVertexBuffer.h"
#include "athena/iphone/IphoneTexture.h"
#include "athena/Mesh.h"
#include "athena/MeshProvider.h"
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

using namespace Athena;

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
        
        m_renderQueue.clear();
        
        const SceneNodePtr& sceneRoot = viewport->GetSceneRoot();
        sceneRoot->TraverseNodes(std::tr1::bind(&CIphoneGraphicDevice::FillRenderQueue, this, std::tr1::placeholders::_1, camera.get()));
        
        for(RenderQueue::const_iterator meshIterator(m_renderQueue.begin());
            meshIterator != m_renderQueue.end(); meshIterator++)
        {
            CMesh* mesh = (*meshIterator);
            DrawMesh(mesh);
        }
	}
    
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, NULL);

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, NULL);
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
    
    CVector2 worldPosition = mesh->GetWorldPosition();
    CVector2 worldScale = mesh->GetWorldScale();
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(worldPosition.x, worldPosition.y, 0);
    glScalef(worldScale.x, worldScale.y, 1);
    
    //Setup material
    {
        MaterialPtr material = mesh->GetMaterial();
        assert(material != NULL);
        CColor color = material->GetColor();
        RENDER_TYPE renderType = material->GetRenderType();
        
        glColor4f(color.r, color.g, color.b, color.a);
        
        if(renderType == RENDER_ONLYCOLOR)
        {
            glActiveTexture(GL_TEXTURE0);
            glDisable(GL_TEXTURE_2D);
            
            glActiveTexture(GL_TEXTURE1);
            glDisable(GL_TEXTURE_2D);
        }
        else if(renderType == RENDER_DIFFUSE)
        {
            TexturePtr diffuseTexture = material->GetTexture(0);
            assert(diffuseTexture);
            
            if(diffuseTexture)
            {
                CIphoneTexture* textureGen = static_cast<CIphoneTexture*>(diffuseTexture.get());
                
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureGen->GetTexture());
            }
            
            glActiveTexture(GL_TEXTURE1);
            glDisable(GL_TEXTURE_2D);
        }
        else if(renderType == RENDER_LIGHTMAPPED)
        {
            TexturePtr diffuseTexture = material->GetTexture(0);
            TexturePtr lightMapTexture = material->GetTexture(1);
            
            if(diffuseTexture)
            {
                CIphoneTexture* textureGen = static_cast<CIphoneTexture*>(diffuseTexture.get());
                
                glActiveTexture(GL_TEXTURE0);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureGen->GetTexture());
            }
            
            if(lightMapTexture)
            {
                CIphoneTexture* textureGen = static_cast<CIphoneTexture*>(lightMapTexture.get());

                glActiveTexture(GL_TEXTURE1);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, textureGen->GetTexture());
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
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    assert(glGetError() == GL_NO_ERROR);
    
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
    
    if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_COLOR)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.colorOffset));
    }
    else
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    assert(glGetError() == GL_NO_ERROR);

    glClientActiveTexture(GL_TEXTURE0);
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

    glClientActiveTexture(GL_TEXTURE1);
    if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_UV1)
    {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.uv1Offset));
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
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    m_drawCallCount++;
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
