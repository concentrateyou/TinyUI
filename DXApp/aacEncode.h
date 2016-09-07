#pragma once
#include "DXFramework.h"
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
private:
	BOOL	Open(const WAVEFORMATEX& wfx, INT bitRate);
	BOOL	Encode(BYTE* bits,LONG size);
	void	Close();
	BOOL	GetSpecificInfo(vector<BYTE>& info);
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	ULONG					m_inputSamples;
	ULONG					m_maxOutputBytes;
	vector<BYTE>			m_output;
	vector<BYTE>			m_header;
};

