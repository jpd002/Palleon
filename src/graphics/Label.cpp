#include "palleon/graphics/Label.h"
#include "palleon/resources/ResourceManager.h"
#include "palleon/graphics/GraphicDevice.h"
#include "Utf8.h"

using namespace Palleon;

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
, m_wordWrapEnabled(true)
, m_size(0, 0)
{

}

CLabel::~CLabel()
{

}

LabelPtr CLabel::Create()
{
	return LabelPtr(new CLabel());
}

void CLabel::SetText(const std::string& text)
{
	m_text = Framework::Utf8::ConvertFrom(text);
	m_dirty = true;
}

void CLabel::SetFont(const CFontDescriptor* font)
{
	m_font = font;
	m_material->SetTexture(0, CResourceManager::GetInstance().GetTexture(font->GetTextureName()));
	m_material->SetAlphaBlendingMode(ALPHA_BLENDING_LERP);
}

void CLabel::SetHorizontalAlignment(HORIZONTAL_ALIGNMENT align)
{
	m_horizontalAlignment = align;
}

void CLabel::SetVerticalAlignment(VERTICAL_ALIGNMENT align)
{
	m_verticalAlignment = align;
}

void CLabel::SetWordWrapEnabled(bool wordWrapEnabled)
{
	m_wordWrapEnabled = wordWrapEnabled;
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
	auto lines = GetLines();
	uint32 currentCharCount = GetCharCount(lines);

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

	TEXTPOSINFO textPosInfo = GetTextPosition(lines);
	float textureWidth = static_cast<float>(m_font->GetTextureWidth());
	float textureHeight = static_cast<float>(m_font->GetTextureHeight());

	unsigned int currentLine = 0;
	float posY = textPosInfo.posY;

	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc = m_vertexBuffer->GetDescriptor();
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_UV0);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	uint8* verticesEnd = vertices + (bufferDesc.GetVertexSize() * m_charCount * 4);
	for(auto lineIterator(std::begin(lines)); lineIterator != std::end(lines); lineIterator++, currentLine++)
	{
		const auto& line = *lineIterator;

		float posX = textPosInfo.linePosX[currentLine];

		for(auto character(std::begin(line)); character != std::end(line); character++)
		{
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

				*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
				*reinterpret_cast<CVector2*>(vertices + uv0VertexItem->offset) = texCoord;
				vertices += bufferDesc.GetVertexSize();
			}

			posX += glyphInfo.xadvance * m_textScale.x;
		}

		posY += static_cast<float>(m_font->GetLineHeight()) * m_textScale.y;
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

CLabel::StringArray CLabel::GetLines() const
{
	StringArray result;

	auto startCharacter = std::begin(m_text);
	for(auto character(std::begin(m_text)); character != std::end(m_text); character++)
	{
		if(*character == '\n')
		{
			result.push_back(std::string(startCharacter, character));
			startCharacter = character + 1;
		}
	}
	if(startCharacter != std::end(m_text))
	{
		result.push_back(std::string(startCharacter, std::end(m_text)));
	}

	//Fit the lines into the bounding box
	if(m_wordWrapEnabled)
	{
		StringArray sourceLines(std::move(result));

		for(auto lineIterator(std::begin(sourceLines));
			lineIterator != std::end(sourceLines); lineIterator++)
		{
			const auto& line = *lineIterator;
			std::string currentLine;
			std::string currentWord;
			std::string currentSeparator;
			for(auto character(std::begin(line)); character != std::end(line); character++)
			{
				if(*character == 0x20)
				{
					CVector2 wordSize = MeasureString(currentWord.c_str());
					CVector2 lineSize = MeasureString(currentLine.c_str());
					if((wordSize.x + lineSize.x) > m_size.x)
					{
						result.push_back(currentLine);
						currentLine = currentWord;
						currentWord.clear();
					}
					else
					{
						currentLine += currentSeparator;
						currentLine += currentWord;
						currentWord.clear();
						currentSeparator = *character;
					}
				}
				else
				{
					currentWord += *character;
				}
			}

			if(currentLine.length() != 0)
			{
				CVector2 wordSize = MeasureString(currentWord.c_str());
				CVector2 lineSize = MeasureString(currentLine.c_str());
				if((wordSize.x + lineSize.x) > m_size.x)
				{
					result.push_back(currentLine);
					result.push_back(currentWord);
				}
				else
				{
					result.push_back(currentLine + currentSeparator + currentWord);
				}
			}
			else if(currentWord.length() != 0)
			{
				result.push_back(currentWord);
			}
		}
	}

	return result;
}

unsigned int CLabel::GetCharCount(const StringArray& lines) const
{
	unsigned int result = 0;
	for(auto lineIterator(std::begin(lines)); lineIterator != std::end(lines); lineIterator++)
	{
		const auto& line = *lineIterator;
		result += line.size();
	}
	return result;
}

CLabel::FloatArray CLabel::GetLineWidths(const StringArray& lines) const
{
	CLabel::FloatArray widths;

	widths.reserve(lines.size());

	for(auto lineIterator(std::begin(lines));
		lineIterator != std::end(lines); lineIterator++)
	{
		auto lineSize = MeasureString(lineIterator->c_str());
		widths.push_back(lineSize.x);
	}

	return widths;
}

float CLabel::GetTextHeight(const StringArray& lines) const
{
	unsigned int lineCount = lines.size();
	return static_cast<float>(lineCount * m_font->GetLineHeight()) * m_textScale.y;
}

CLabel::TEXTPOSINFO CLabel::GetTextPosition(const StringArray& lines) const
{
	TEXTPOSINFO result;

	FloatArray lineWidths = GetLineWidths(lines);
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

	float textHeight = GetTextHeight(lines);
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

CVector2 CLabel::MeasureString(const char* string) const
{
	size_t charCount = strlen(string);
	float currentWidth = 0;
	for(unsigned int i = 0; i < charCount; i++)
	{
		uint8 character = static_cast<uint8>(string[i]);
		CFontDescriptor::GLYPHINFO glyphInfo = m_font->GetGlyphInfo(character);
		currentWidth += glyphInfo.xadvance * m_textScale.x;
	}
	return CVector2(currentWidth, static_cast<float>(m_font->GetLineHeight()));
}
