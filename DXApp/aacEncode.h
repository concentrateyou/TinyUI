#pragma once
#include "Utility.h"
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
	DWORD	GetLatestPTS() const;
public:
	virtual void OnDone(BYTE*, INT);
public:
	Event<void(BYTE*, INT)> EVENT_DONE;
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	DWORD					m_inputSamples;
	DWORD					m_maxOutputBytes;
	DWORD					m_duration;
	DWORD					m_dwINC;
	DWORD					m_dwPTS;
	TinyScopedArray<BYTE>	m_bits;
	WAVEFORMATEX			m_wfx;
};

