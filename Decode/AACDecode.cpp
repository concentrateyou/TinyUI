#include "stdafx.h"
#include "AACDecode.h"

namespace Decode
{
	AACDecode::AACDecode()
		:m_handle(NULL)
	{
	}
	AACDecode::~AACDecode()
	{
		Close();
	}
	BOOL AACDecode::Open(BYTE* adts, LONG size, WORD wBitsPerSample)
	{
		this->Close();
		ZeroMemory(&m_sMFT, sizeof(m_sMFT));
		m_handle = NeAACDecOpen();
		if (!m_handle)
			return FALSE;
		NeAACDecConfigurationPtr cfg;
		cfg = NeAACDecGetCurrentConfiguration(m_handle);
		cfg->dontUpSampleImplicitSBR = 1;
		NeAACDecSetConfiguration(m_handle, cfg);
		ULONG sampleRate = 0;
		BYTE channel = 0;
		if (NeAACDecInit2(m_handle, adts, size, &sampleRate, &channel) != 0)
			goto AAC_ERROR;
		m_sMFT.cbSize = 0;
		m_sMFT.nSamplesPerSec = sampleRate;
		m_sMFT.nChannels = channel;
		m_sMFT.wBitsPerSample = wBitsPerSample;
		m_sMFT.nBlockAlign = wBitsPerSample * channel / 8;
		m_sMFT.nAvgBytesPerSec = m_sMFT.nSamplesPerSec * m_sMFT.nBlockAlign;
		m_sMFT.wFormatTag = WAVE_FORMAT_PCM;
		return TRUE;
	AAC_ERROR:
		NeAACDecClose(m_handle);
		m_handle = NULL;
		return FALSE;
	}
	BOOL AACDecode::Open(WORD wBitsPerSample, WORD wSampleRate)
	{
		m_handle = NeAACDecOpen();
		if (!m_handle)
			goto AAC_ERROR;
		NeAACDecConfiguration* config = NeAACDecGetCurrentConfiguration(m_handle);
		if (!config)
			return FALSE;
		config->outputFormat = FAAD_FMT_FLOAT;
		switch (wBitsPerSample)
		{
		case 16:
			config->outputFormat = FAAD_FMT_16BIT;
			break;
		case 24:
			config->outputFormat = FAAD_FMT_24BIT;
			break;
		case 32:
			config->outputFormat = FAAD_FMT_32BIT;
			break;
		}
		config->defSampleRate = wSampleRate;
		config->defObjectType = LOW;
		config->downMatrix = 1;
		config->useOldADTSFormat = 0;
		if (NeAACDecSetConfiguration(m_handle, config) != 0)
			goto AAC_ERROR;
		return TRUE;
	AAC_ERROR:
		NeAACDecClose(m_handle);
		m_handle = NULL;
		return FALSE;
	}
	WAVEFORMATEX* AACDecode::GetFormat()
	{
		return &m_sMFT;
	}
	NeAACDecHandle AACDecode::GetHandle()
	{
		return m_handle;
	}
	BOOL AACDecode::Decode(SampleTag& tag, BYTE*& bo, LONG& so)
	{
		BOOL bRes = TRUE;
		if (!m_handle)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		bo = reinterpret_cast<BYTE*>(NeAACDecDecode(m_handle, &m_frame, tag.bits, tag.size));
		if (m_frame.error != 0 || m_frame.samples <= 0)
		{
			bRes = FALSE;
			goto _ERROR;
		}
		so = m_frame.samples * m_frame.channels;
	_ERROR:
		SAFE_DELETE_ARRAY(tag.bits);
		return bRes;
	}
	BOOL AACDecode::Close()
	{
		ZeroMemory(&m_sMFT, sizeof(m_sMFT));
		ZeroMemory(&m_frame, sizeof(m_frame));
		if (m_handle != NULL)
		{
			NeAACDecClose(m_handle);
			m_handle = NULL;
		}
		return TRUE;
	}
}

