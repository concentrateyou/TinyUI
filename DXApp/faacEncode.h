#pragma once
#include "Utility.h"
extern "C"
{
#include "faac.h"
#include "faaccfg.h"
}
#pragma comment(lib,"libfaac.lib")

#define  AAC_TIMEBASE 1024
#define  AAC_TIMEDEN  1000

class FaacEncode
{
public:
	FaacEncode();
	~FaacEncode();
public:
	BOOL	Open(const WAVEFORMATEX& wfx, INT audioRate);
	BOOL	Encode(BYTE* bits, LONG size, DWORD& dwINC);
	void	Close();
	BOOL	GetSpecificInfo(vector<BYTE>& info);
public:
	virtual void OnDone(BYTE*, LONG, const MediaTag&);
	Event<void(BYTE*, LONG, const MediaTag&)> EVENT_DONE;
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	DWORD					m_inputSamples;
	DWORD					m_maxOutputBytes;
	DWORD					m_dwINC;
	DWORD					m_dwPTS;
	TinyScopedArray<BYTE>	m_bits;
	WAVEFORMATEX			m_wfx;
};

