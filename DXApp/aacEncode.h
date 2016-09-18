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
	BOOL	Open(const WAVEFORMATEX& wfx);
	BOOL	Encode(BYTE* bits, LONG size, EncoderPacket& packet);
	void	Close();
	BOOL	GetSpecificInfo(vector<BYTE>& info);
private:
	faacEncHandle			m_aac;
	faacEncConfigurationPtr	m_config;
	ULONG					m_inputSamples;
	ULONG					m_maxOutputBytes;
	vector<BYTE>			m_output;
	WAVEFORMATEX			m_wfx;
	LONG					m_timestamp;
};

