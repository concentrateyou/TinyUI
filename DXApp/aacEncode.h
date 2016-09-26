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

class aacEncode
{
public:
	aacEncode();
	~aacEncode();
public:
	BOOL	Open(const WAVEFORMATEX& wfx, INT audioRate);
	BOOL	Encode(BYTE* bits, INT size);
	void	Close();
	BOOL	GetSpecificInfo(vector<BYTE>& info);
	DWORD	GetLatestPTS() const;
public:
	virtual void OnDone(TinyScopedReferencePtr<Sample>& sample);
public:
	Event<void(TinyScopedReferencePtr<Sample>&)> EVENT_DONE;
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	DWORD					m_inputSamples;
	DWORD					m_maxOutputBytes;
	DWORD					m_dwTime;
	DWORD					m_dwINC;
	DWORD					m_dwPTS;
	TinyScopedArray<BYTE>	m_bits;
	WAVEFORMATEX			m_wfx;
	FILE*					m_aacFile;
};

