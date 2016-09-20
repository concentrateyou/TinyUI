#include "stdafx.h"
#include "aacEncode.h"


aacEncode::aacEncode()
{
}


aacEncode::~aacEncode()
{
}

BOOL aacEncode::GetSpecificInfo(vector<BYTE>& info)
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

BOOL aacEncode::Open(const WAVEFORMATEX& wfx, INT audioRate)
{
	m_wfx = wfx;
	m_aac = faacEncOpen(wfx.nSamplesPerSec, wfx.nChannels, &m_inputSamples, &m_maxOutputBytes);
	if (!m_aac)
		return FALSE;
	m_config = faacEncGetCurrentConfiguration(m_aac);
	m_config->aacObjectType = LOW;
	m_config->quantqual = 100;
	switch (wfx.wBitsPerSample)
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
	default:
		m_config->inputFormat = FAAC_INPUT_FLOAT;
		break;
	}
	m_config->mpegVersion = MPEG4;
	m_config->useLfe = 0;
	m_config->outputFormat = 0;
	m_config->bitRate = audioRate * 1000 / wfx.nChannels;//±ÈÌØÎ»
	if (!faacEncSetConfiguration(m_aac, m_config))
		return FALSE;
	m_bits.Reset(new BYTE[m_maxOutputBytes]);
	return TRUE;
}
BOOL aacEncode::Encode(BYTE* bits)
{
	if (!bits)
		return FALSE;
	INT size = faacEncEncode(m_aac, (int32_t*)bits, m_inputSamples, m_bits, m_maxOutputBytes);
	if (size > 0)
	{
		OnDone(m_bits, size);
		return TRUE;
	}
	return FALSE;
}
void aacEncode::Close()
{
	if (m_aac)
	{
		faacEncClose(m_aac);
		m_aac = NULL;
	}
}
void aacEncode::OnDone(BYTE* bits, INT size)
{
	EVENT_DONE(bits, size);
}
