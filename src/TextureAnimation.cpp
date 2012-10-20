#include "athena/TextureAnimation.h"
#include "athena/Mesh.h"

using namespace Athena;

CTextureAnimation::CTextureAnimation()
: m_length(0)
{

}

CTextureAnimation::~CTextureAnimation()
{

}

void CTextureAnimation::AddFrame(const TexturePtr& texture, float length)
{
	FRAME frame;
	frame.texture	= texture;
	frame.length	= length;
	m_frames.push_back(frame);

	m_length += length;
}

void CTextureAnimation::Animate(CSceneNode* target, float t) const
{
	auto mesh = dynamic_cast<CMesh*>(target);
	if(!mesh) return;
	auto material = mesh->GetMaterial();
	if(!material) return;

	for(auto frameIterator(m_frames.begin());
		frameIterator != m_frames.end(); frameIterator++)
	{
		const auto& frame(*frameIterator);
		if(t <= frame.length)
		{
			material->SetTexture(0, frame.texture);
			return;
		}
		t -= frame.length;
	}

	if(m_frames.size() != 0)
	{
		const auto& frame(*m_frames.rbegin());
		material->SetTexture(0, frame.texture);
	}
}

float CTextureAnimation::GetLength() const
{
	return m_length;
}
