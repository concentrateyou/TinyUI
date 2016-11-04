#include "stdafx.h"
#include "FaacEncode.h"


FaacEncode::FaacEncode()
	:m_inputSamples(0),
	m_maxOutputBytes(0),
	m_dwINC(0),
	m_dwPTS(0)
{
}

FaacEncode::~FaacEncode()
{

}

BOOL FaacEncode::GetSpecificInfo(vector<BYTE>& info)
{
	BYTE* buffer = NULL;
	ULONG size = 0;
	if (faacEncGetDecoderSpecificInfo(m_aac, &buffer, &size) != 0)
	{
		SAFE_FREE(buffer);
		return FALSE;
	}
	info.resize(size);
	memcpy(&info[0], buffer, size);
	SAFE_FREE(buffer);
	return TRUE;
}
BOOL FaacEncode::Open(const WAVEFORMATEX& waveFMT, INT audioRate, BOOL bAllowF)
{
	Close();
	m_bAllowF = bAllowF;
	m_waveFMT = waveFMT;
	m_aac = faacEncOpen(waveFMT.nSamplesPerSec, waveFMT.nChannels, &m_inputSamples, &m_maxOutputBytes);
	if (!m_aac)
		return FALSE;
	//AAC固定1024
	m_dwPTS = AAC_TIMEBASE * AAC_TIMEDEN / waveFMT.nSamplesPerSec;//播放一帧时间
	m_config = faacEncGetCurrentConfiguration(m_aac);
	if (m_bAllowF)
	{
		m_config->inputFormat = FAAC_INPUT_FLOAT;
	}
	else
	{
		switch (waveFMT.wBitsPerSample)
		{
		case 16:
			m_config->inputFormat = FAAC_INPUT_16BIT;
			break;
		case 24:
			m_config->inputFormat = FAAC_INPUT_24BIT;
			break;
		case 32:
			m_config->inputFormat = FAAC_INPUT_32BIT;
			break;
		}
	}
	m_config->quantqual = 100;
	m_config->mpegVersion = MPEG4;
	m_config->aacObjectType = LOW;
	m_config->useTns = 0;
	m_config->useLfe = 0;
	m_config->bandWidth = 0;
	m_config->outputFormat = 1;//ADT
	m_config->shortctl = SHORTCTL_NORMAL;
	m_config->bitRate = audioRate * 1000 / waveFMT.nChannels;//比特位
	INT iRes = faacEncSetConfiguration(m_aac, m_config);
	if (!iRes)
		return FALSE;
	m_bits.Reset(new BYTE[m_maxOutputBytes]);
	return TRUE;
}
BOOL FaacEncode::Encode(BYTE* bits, LONG size, DWORD& dwINC)
{
	if (!bits)
		return FALSE;
	INT s = faacEncEncode(m_aac, (int32_t*)bits, m_inputSamples, m_bits, m_maxOutputBytes);
	if (s > 0)
	{
		MediaTag tag;
		tag.dwPTS = m_dwPTS;
		tag.dwDTS = 0;
		tag.dwType = 1;
		tag.dwTime = timeGetTime();
		tag.dwFlag = 0;
		tag.dwINC = ++m_dwINC;
		OnDone(m_bits, s, tag);
		return TRUE;
	}
	return FALSE;
}
void FaacEncode::Close()
{
	if (m_aac)
	{
		faacEncClose(m_aac);
		m_aac = NULL;
	}
}
void FaacEncode::OnDone(BYTE* bits, LONG size, const MediaTag& tag)
{
	EVENT_DONE(bits, size, tag);
}
