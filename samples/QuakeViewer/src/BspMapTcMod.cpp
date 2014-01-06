#include "BspMapTcMod.h"

CBspMapTcMod::CBspMapTcMod()
{
	m_matrix = CMatrix4::MakeIdentity();
}

const CMatrix4& CBspMapTcMod::GetMatrix() const
{
	return m_matrix;
}

//-------------------------------------------------------------
//Scroll
//-------------------------------------------------------------
CBspMapTcMod_Scroll::CBspMapTcMod_Scroll(float speedS, float speedT)
: m_speedS(speedS)
, m_speedT(speedT)
, m_s(0)
, m_t(0)
{

}

void CBspMapTcMod_Scroll::Update(float dt)
{
	m_matrix = CMatrix4::MakeTranslation(m_s, m_t, 0);

	m_s += m_speedS * dt;
	m_t += m_speedT * dt;
}

//-------------------------------------------------------------
//Scale
//-------------------------------------------------------------
CBspMapTcMod_Scale::CBspMapTcMod_Scale(float s, float t)
{
	m_matrix = CMatrix4::MakeScale(s, t, 1);
}

void CBspMapTcMod_Scale::Update(float dt)
{

}

//-------------------------------------------------------------
//Rotate
//-------------------------------------------------------------
CBspMapTcMod_Rotate::CBspMapTcMod_Rotate(float speedAlpha)
: m_speedAlpha((speedAlpha * M_PI) / 180.f)
, m_alpha(0)
{

}

void CBspMapTcMod_Rotate::Update(float dt)
{
	CMatrix4 trans		= CMatrix4::MakeTranslation(-0.5f, -0.5f, 0.f);
	CMatrix4 transInv	= CMatrix4::MakeTranslation( 0.5f,  0.5f, 0.f);
	m_matrix = trans * CMatrix4::MakeAxisZRotation(m_alpha) * transInv;

	m_alpha += m_speedAlpha * dt;
}

//-------------------------------------------------------------
//Turb
//-------------------------------------------------------------

CBspMapTcMod_Turb::CBspMapTcMod_Turb(float amplitude, float freq)
: m_amplitude(amplitude)
, m_freq(freq)
, m_alpha(0)
, m_currS(0)
, m_currT(0)
{

}

void CBspMapTcMod_Turb::Update(float dt)
{
	m_currS = (m_amplitude / 16.f) * sin(m_alpha * m_freq * (2.f * M_PI));
	m_currT = (m_amplitude / 16.f) * cos(m_alpha * m_freq * (2.f * M_PI));

	m_matrix = CMatrix4::MakeScale(1 + m_currS, 1 + m_currT, 1);

	m_alpha += dt;
}

//-------------------------------------------------------------
//Stretch
//-------------------------------------------------------------

float EvaluateWaveFunction(const BSPMAPWAVEPARAMS& wave, float alpha)
{
	switch(wave.type)
	{
	case BSPMAPWAVEPARAMS::WAVE_SIN:
		return wave.base + wave.amplitude * sin(alpha * (wave.freq * (2 * M_PI)) + wave.phase);
		break;
	default:
		return wave.base;
		break;
	}
}

CBspMapTcMod_Stretch::CBspMapTcMod_Stretch(const BSPMAPWAVEPARAMS& wave)
: m_wave(wave)
, m_alpha(0)
{

}

void CBspMapTcMod_Stretch::Update(float dt)
{
	float value = EvaluateWaveFunction(m_wave, m_alpha);

	CMatrix4 trans		= CMatrix4::MakeTranslation(-0.5f, -0.5f, 0.f);
	CMatrix4 transInv	= CMatrix4::MakeTranslation( 0.5f,  0.5f, 0.f);
	m_matrix = trans * CMatrix4::MakeScale(1 / value, 1 / value, 1) * transInv;

	m_alpha += dt;
}
