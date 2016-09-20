#pragma once
#include "BaseEncode.h"
extern "C"
{
#include "faac.h"
#include "faaccfg.h"
}
#pragma comment(lib,"libfaac.lib")

class aacEncode
{
public:
	aacEncode();
	~aacEncode();
public:
	BOOL	Open(const WAVEFORMATEX& wfx, INT audioRate);
	BOOL	Encode(BYTE* bits);
	void	Close();
	BOOL	GetSpecificInfo(vector<BYTE>& info);
public:
	virtual void OnDone(BYTE*, INT);
public:
	Event<void(BYTE*, INT)> EVENT_DONE;
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	ULONG					m_inputSamples;
	ULONG					m_maxOutputBytes;
	TinyScopedArray<BYTE>	m_bits;
	WAVEFORMATEX			m_wfx;
};

