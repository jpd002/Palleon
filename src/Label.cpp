#include "athena/Label.h"
#include "athena/ResourceManager.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

static const float s_positions[4 * 3] =
{
	0, 0, 0,
	1, 0, 0,
	0, 1, 0,
	1, 1, 0
};

static const float s_texCoords[4 * 2] =
{
	0, 0,
	1, 0,
	0, 1,
	1, 1
};

CLabel::CLabel()
: m_font(NULL)
, m_horizontalAlignment(HORIZONTAL_ALIGNMENT_LEFT)
, m_verticalAlignment(VERTICAL_ALIGNMENT_TOP)
, m_dirty(false)
, m_charCount(0)
{
    
}

CLabel::~CLabel()
{

}

LabelPtr CLabel::Create()
{
	return LabelPtr(new CLabel());
}

void CLabel::SetText(const char* text)
{
	m_text = text;
	m_dirty = true;
}

void CLabel::SetFont(const CFontDescriptor* font)
{
    m_font = font;
	m_material->SetTexture(0, CResourceManager::GetInstance().GetTexture(font->GetTextureName()));
	m_material->SetIsTransparent(true);
}

void CLabel::SetHorizontalAlignment(HORIZONTAL_ALIGNMENT align)
{
    m_horizontalAlignment = align;
}

void CLabel::SetVerticalAlignment(VERTICAL_ALIGNMENT align)
{
    m_verticalAlignment = align;
}

void CLabel::SetSize(const CVector2& size)
{
    m_size = size;
}

void CLabel::Update(float dt)
{
	if(m_dirty)
	{
		BuildVertexBuffer();
		m_dirty = false;
	}
}

void CLabel::BuildVertexBuffer()
{
	uint32 currentCharCount = m_text.length();

	//We need to update the vertex buffer
	if(!m_vertexBuffer || (currentCharCount > m_charCount))
	{
		m_charCount = currentCharCount;

		VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(m_charCount * 4, m_charCount * 6, 
			VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);

		m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
		m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);
	}

	m_primitiveCount = currentCharCount * 2;

	CVector2 textPosition(GetTextPosition());
	float textureWidth = static_cast<float>(m_font->GetTextureWidth());
	float textureHeight = static_cast<float>(m_font->GetTextureHeight());

	float posX = textPosition.x;
	float posY = textPosition.y;

	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc = m_vertexBuffer->GetDescriptor(); 

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int i = 0; i < currentCharCount; i++)
	{
		CFontDescriptor::GLYPHINFO glyphInfo = m_font->GetGlyphInfo(m_text[i]);

		for(unsigned int j = 0; j < 4; j++)
		{
			CVector3 position(&s_positions[j * 3]);
			position.x *= glyphInfo.dx;
			position.y *= glyphInfo.dy;
			position.x += posX + glyphInfo.xoffset;
			position.y += posY + glyphInfo.yoffset;

			CVector2 texCoord(&s_texCoords[j * 2]);
			texCoord.x *= glyphInfo.dx / textureWidth;
			texCoord.y *= glyphInfo.dy / textureHeight;
			texCoord.x += glyphInfo.x / textureWidth;
			texCoord.y += glyphInfo.y / textureHeight;

			*reinterpret_cast<CVector3*>(vertices + bufferDesc.posOffset) = position;
			*reinterpret_cast<CVector2*>(vertices + bufferDesc.uv0Offset) = texCoord;
			vertices += bufferDesc.GetVertexSize();
		}

		posX += glyphInfo.xadvance;
	}
	m_vertexBuffer->UnlockVertices();

	uint16* indices = m_vertexBuffer->LockIndices();
	for(unsigned int i = 0; i < currentCharCount; i++)
	{
		indices[(i * 6) + 0] = (i * 4) + 0;
		indices[(i * 6) + 1] = (i * 4) + 1;
		indices[(i * 6) + 2] = (i * 4) + 2;
		indices[(i * 6) + 3] = (i * 4) + 2;
		indices[(i * 6) + 4] = (i * 4) + 1;
		indices[(i * 6) + 5] = (i * 4) + 3;
	}
	m_vertexBuffer->UnlockIndices();
}

CVector2 CLabel::GetTextExtents() const
{
    CVector2 result;
    result.x = 0;
    result.y = static_cast<float>(m_font->GetLineHeight());
    
	unsigned int charCount = m_text.length();
	for(unsigned int i = 0; i < charCount; i++)
	{
		CFontDescriptor::GLYPHINFO glyphInfo = m_font->GetGlyphInfo(m_text[i]);
        result.x += glyphInfo.xadvance;
	}
    
    return result;
}

CVector2 CLabel::GetTextPosition() const
{
	float posX = 0;
	float posY = 0;
	CVector2 extents;

    switch(m_horizontalAlignment)
    {
        case HORIZONTAL_ALIGNMENT_LEFT:
            posX = 0;
            break;
        case HORIZONTAL_ALIGNMENT_RIGHT:
            posX = m_size.x - extents.x;
            break;
        case HORIZONTAL_ALIGNMENT_CENTER:
            posX = (m_size.x - extents.x) / 2;
            break;
    }
    
    switch(m_verticalAlignment)
    {
        case VERTICAL_ALIGNMENT_TOP:
            posY = 0;
            break;
        case VERTICAL_ALIGNMENT_BOTTOM:
            posY = m_size.y - extents.y;
            break;
        case VERTICAL_ALIGNMENT_CENTER:
            posY = (m_size.y - extents.y) / 2;
            break;
    }

	return CVector2(posX, posY);
}
