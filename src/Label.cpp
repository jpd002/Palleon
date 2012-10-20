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
, m_textScale(1, 1)
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

void CLabel::SetPosition(const CVector3& position)
{
	CMesh::SetPosition(position);
}

void CLabel::SetSize(const CVector2& size)
{
	m_size = size;
	m_dirty = true;
}

void CLabel::SetTextScale(const CVector2& textScale)
{
	m_textScale = textScale;
	m_dirty = true;
}

void CLabel::Update(float dt)
{
	CSceneNode::Update(dt);
	if(m_dirty)
	{
		BuildVertexBuffer();
		m_dirty = false;
	}
}

void CLabel::BuildVertexBuffer()
{
	uint32 currentCharCount = GetCharCount();

	m_primitiveCount = currentCharCount * 2;

	if(m_primitiveCount == 0) return;

	//We need to update the vertex buffer
	if(!m_vertexBuffer || (currentCharCount > m_charCount))
	{
		m_charCount = currentCharCount;

		VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(m_charCount * 4, m_charCount * 6, 
			VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);

		m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
		m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);
	}

	TEXTPOSINFO textPosInfo = GetTextPosition();
	float textureWidth = static_cast<float>(m_font->GetTextureWidth());
	float textureHeight = static_cast<float>(m_font->GetTextureHeight());

	unsigned int currentLine = 0;
	float posX = (currentLine < textPosInfo.linePosX.size()) ? textPosInfo.linePosX[currentLine] : 0;
	float posY = textPosInfo.posY;

	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc = m_vertexBuffer->GetDescriptor(); 

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	uint8* verticesEnd = vertices + (bufferDesc.GetVertexSize() * m_charCount * 4);
	for(auto character(std::begin(m_text)); character != std::end(m_text); character++)
	{
		if(*character == '\n')
		{
			currentLine++;
			posX = (currentLine < textPosInfo.linePosX.size()) ? textPosInfo.linePosX[currentLine] : 0;
			posY += static_cast<float>(m_font->GetLineHeight()) * m_textScale.y;
			continue;
		}

		CFontDescriptor::GLYPHINFO glyphInfo = m_font->GetGlyphInfo(static_cast<uint8>(*character));

		for(unsigned int j = 0; j < 4; j++)
		{
			CVector3 position(&s_positions[j * 3]);
			position.x *= glyphInfo.dx * m_textScale.x;
			position.y *= glyphInfo.dy * m_textScale.y;
			position.x += posX + glyphInfo.xoffset * m_textScale.x;
			position.y += posY + glyphInfo.yoffset * m_textScale.y;

			CVector2 texCoord(&s_texCoords[j * 2]);
			texCoord.x *= glyphInfo.dx / textureWidth;
			texCoord.y *= glyphInfo.dy / textureHeight;
			texCoord.x += glyphInfo.x / textureWidth;
			texCoord.y += glyphInfo.y / textureHeight;

			*reinterpret_cast<CVector3*>(vertices + bufferDesc.posOffset) = position;
			*reinterpret_cast<CVector2*>(vertices + bufferDesc.uv0Offset) = texCoord;
			vertices += bufferDesc.GetVertexSize();
		}

		posX += glyphInfo.xadvance * m_textScale.x;
	}
	assert(vertices <= verticesEnd);
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

unsigned int CLabel::GetCharCount() const
{
	unsigned int result = 0;
	for(auto character(std::begin(m_text)); character != std::end(m_text); character++)
	{
		if(*character == '\n') continue;
		result++;
	}
	return result;
}

unsigned int CLabel::GetLineCount() const
{
	unsigned int charCount = m_text.length();
	if(charCount == 0) return 0;
	unsigned int result = 1;
	for(unsigned int i = 0; i < charCount; i++)
	{
		char character = m_text[i];
		if(character == '\n') result++;
	}
	return result;
}

CLabel::FloatArray CLabel::GetLineWidths() const
{
	CLabel::FloatArray widths;

	unsigned int lineCount = GetLineCount();
	if(lineCount == 0) return widths;

	widths.reserve(lineCount);

	unsigned int charCount = m_text.length();
	float currentWidth = 0;
	for(unsigned int i = 0; i < charCount; i++)
	{
		uint8 character = static_cast<uint8>(m_text[i]);
		if(character == '\n')
		{
			widths.push_back(currentWidth);
			currentWidth = 0;
			continue;
		}
		CFontDescriptor::GLYPHINFO glyphInfo = m_font->GetGlyphInfo(character);
		currentWidth += glyphInfo.xadvance * m_textScale.x;
	}

	widths.push_back(currentWidth);
	assert(widths.size() == lineCount);

	return widths;
}

float CLabel::GetTextHeight() const
{
	unsigned int lineCount = GetLineCount();
	return static_cast<float>(lineCount * m_font->GetLineHeight()) * m_textScale.y;
}

CVector2 CLabel::GetTextExtents() const
{
	FloatArray lineWidths = GetLineWidths();

	CVector2 result;
	result.x = 0;
	for(unsigned int i = 0; i < lineWidths.size(); i++)
	{
		result.x = std::max<float>(lineWidths[i], result.x);
	}
	result.y = GetTextHeight();

	return result;
}

CLabel::TEXTPOSINFO CLabel::GetTextPosition() const
{
	TEXTPOSINFO result;

	FloatArray lineWidths = GetLineWidths();
	result.linePosX.reserve(lineWidths.size());
	for(unsigned int i = 0; i < lineWidths.size(); i++)
	{
		float posX = 0;
		float lineWidth = lineWidths[i];
		switch(m_horizontalAlignment)
		{
			case HORIZONTAL_ALIGNMENT_LEFT:
				posX = 0;
				break;
			case HORIZONTAL_ALIGNMENT_RIGHT:
				posX = m_size.x - lineWidth;
				break;
			case HORIZONTAL_ALIGNMENT_CENTER:
				posX = static_cast<float>(static_cast<int>(m_size.x - lineWidth) / 2);
				break;
		}
		result.linePosX.push_back(posX);
	}

	float textHeight = GetTextHeight();
	float posY = 0;
	switch(m_verticalAlignment)
	{
		case VERTICAL_ALIGNMENT_TOP:
			posY = 0;
			break;
		case VERTICAL_ALIGNMENT_BOTTOM:
			posY = m_size.y - textHeight;
			break;
		case VERTICAL_ALIGNMENT_CENTER:
			posY = static_cast<float>(static_cast<int>(m_size.y - textHeight) / 2);
			break;
	}
	result.posY = posY;

	return result;
}
