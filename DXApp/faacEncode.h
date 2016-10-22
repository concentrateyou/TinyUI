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
	BOOL	Encode(BYTE* bits, LONG size,DWORD& dwINC);
	void	Close();
	BOOL	GetSpecificInfo(vector<BYTE>& info);
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	DWORD					m_inputSamples;
	DWORD					m_maxOutputBytes;
	DWORD					m_dwINC;
	TinyScopedArray<BYTE>	m_bits;
	WAVEFORMATEX			m_wfx;
};

