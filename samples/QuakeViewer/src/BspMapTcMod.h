#ifndef _BSPMAPTCMOD_H_
#define _BSPMAPTCMOD_H_
//-------------------------------------------------------------
#include "PalleonEngine.h"
//-------------------------------------------------------------
class CBspMapTcMod
{
public:
							CBspMapTcMod();
	virtual					~CBspMapTcMod() {}

	virtual void			Update(float) = 0;
	const CMatrix4&			GetMatrix() const;

protected:
	CMatrix4				m_matrix;
};

typedef std::shared_ptr<CBspMapTcMod> BspMapTcModPtr;

//-------------------------------------------------------------
class CBspMapTcMod_Scroll : public CBspMapTcMod
{
public:
							CBspMapTcMod_Scroll(float, float);
	virtual					~CBspMapTcMod_Scroll() {}

	virtual void			Update(float);

private:
	float					m_s;
	float					m_t;

	float					m_speedS;
	float					m_speedT;
};
//-------------------------------------------------------------
class CBspMapTcMod_Scale : public CBspMapTcMod
{
public:
							CBspMapTcMod_Scale(float, float);
	virtual					~CBspMapTcMod_Scale() {}

	virtual void			Update(float);
};
//-------------------------------------------------------------
class CBspMapTcMod_Rotate : public CBspMapTcMod
{
public:
							CBspMapTcMod_Rotate(float);
	virtual					~CBspMapTcMod_Rotate() {}

	virtual void			Update(float);

private:
	float					m_alpha;

	float					m_speedAlpha;
};
//-------------------------------------------------------------
class CBspMapTcMod_Turb : public CBspMapTcMod
{
public:
							CBspMapTcMod_Turb(float, float);
	virtual					~CBspMapTcMod_Turb() {}

	virtual void			Update(float);

private:
	float					m_alpha;
	float					m_currS;
	float					m_currT;

	float					m_amplitude;
	float					m_freq;
};
//-------------------------------------------------------------
struct BSPMAPWAVEPARAMS
{
	enum WAVE_TYPE
	{
		WAVE_SIN,
	};

	WAVE_TYPE	type;
	float		base;
	float		amplitude;
	float		phase;
	float		freq;
};
//-------------------------------------------------------------
class CBspMapTcMod_Stretch : public CBspMapTcMod
{
public:
							CBspMapTcMod_Stretch(const BSPMAPWAVEPARAMS&);
	virtual					~CBspMapTcMod_Stretch() {}

	virtual void			Update(float);

private:
	BSPMAPWAVEPARAMS		m_wave;

	float					m_alpha;
};
//-------------------------------------------------------------
#endif
