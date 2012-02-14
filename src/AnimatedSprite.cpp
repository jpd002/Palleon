#include "Athena/AnimatedSprite.h"

using namespace Athena;

CAnimatedSprite::CAnimatedSprite()
: m_state(STATE_STOPPED)
, m_currentFrameIndex(0)
, m_currentFrameTime(0)
{

}

CAnimatedSprite::~CAnimatedSprite()
{

}

AnimatedSpritePtr CAnimatedSprite::Create()
{
	return std::make_shared<CAnimatedSprite>();
}

void CAnimatedSprite::Update(float dt)
{
	if(m_state == STATE_PLAYING_ONCE || m_state == STATE_PLAYING_LOOP)
	{
		m_currentFrameTime -= dt;
		if(m_currentFrameTime <= 0)
		{
			m_currentFrameIndex++;
			if(m_currentFrameIndex == m_frames.size())
			{
				if(m_state == STATE_PLAYING_ONCE)
				{
					//We're done
					m_state = STATE_STOPPED;
					AnimationOver();
				}
				else
				{
					m_currentFrameIndex = 0;
				}
			}

			if(m_state != STATE_STOPPED)
			{
				m_currentFrameTime = m_frames[m_currentFrameIndex].timeLength;
				UpdateTexture();
			}
		}
	}

	CSprite::Update(dt);
}

void CAnimatedSprite::PlayOnce()
{
	assert(m_frames.size() != 0);
	assert(m_state == STATE_STOPPED);
	if(m_frames.size() == 0) return;

	m_state = STATE_PLAYING_ONCE;
	m_currentFrameIndex = 0;

	m_currentFrameTime = m_frames[m_currentFrameIndex].timeLength;
	UpdateTexture();
}

void CAnimatedSprite::PlayLoop()
{
	assert(m_frames.size() != 0);
	assert(m_state == STATE_STOPPED);
	if(m_frames.size() == 0) return;

	m_state = STATE_PLAYING_LOOP;
	m_currentFrameIndex = 0;

	m_currentFrameTime = m_frames[m_currentFrameIndex].timeLength;
	UpdateTexture();
}

void CAnimatedSprite::AddFrame(const TexturePtr& texture, float timeLength)
{
	FRAME frame;
	frame.texture		= texture;
	frame.timeLength	= timeLength;
	m_frames.push_back(frame);

	if(m_frames.size() == 1)
	{
		UpdateTexture();
	}
}

void CAnimatedSprite::UpdateTexture()
{
	const FRAME& frame(m_frames[m_currentFrameIndex]);
	m_material->SetTexture(0, frame.texture);
}
