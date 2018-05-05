#include "stdafx.h"
#include "AACDecoder.h"

namespace Decode
{
	static INT AACSampleRates[16] =
	{
		96000, 88200, 64000, 48000, 44100, 32000,
		24000, 22050, 16000, 12000, 11025, 8000, 7350,
		0, 0, 0
	};

	static INT GetSampleRate(const BYTE* adts, LONG size)
	{
		if (!adts || size < 2)
			return -1;
		TinyBitReader reader;
		reader.Initialize(adts, size);
		reader.SkipBits(5);
		INT samplingFrequencyIndex = 0;
		reader.ReadBits(4, &samplingFrequencyIndex);
		if (samplingFrequencyIndex >= 16)
			return -1;
		return AACSampleRates[samplingFrequencyIndex];
	}

	//////////////////////////////////////////////////////////////////////////
	AACDecoder::AACDecoder()
		:m_handle(NULL)
	{
	}
	AACDecoder::~AACDecoder()
	{
		Close();
	}
	BOOL AACDecoder::Open(BYTE* adts, LONG size, WORD wBitsPerSample)
	{
		ZeroMemory(&m_sMFT, sizeof(m_sMFT));
		m_handle = NeAACDecOpen();
		if (!m_handle)
			return FALSE;
		NeAACDecConfigurationPtr cfg;
		cfg = NeAACDecGetCurrentConfiguration(m_handle);
		cfg->dontUpSampleImplicitSBR = 1;
		NeAACDecSetConfiguration(m_handle, cfg);
		ULONG sampleRate1 = GetSampleRate(adts, size);
		ULONG sampleRate2 = 0;
		BYTE channel = 0;
		//AAC-LC <=24000此方法采样率错误
		if (NeAACDecInit2(m_handle, adts, size, &sampleRate2, &channel) != 0)
			goto AAC_ERROR;
		m_sMFT.cbSize = 0;
		m_sMFT.nSamplesPerSec = sampleRate2;//sampleRate1 <= 24000 ? sampleRate2 / 2 : sampleRate2;
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
	BOOL AACDecoder::Open(WORD wBitsPerSample, WORD wSampleRate)
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
	WAVEFORMATEX* AACDecoder::GetFormat()
	{
		return &m_sMFT;
	}
	NeAACDecHandle AACDecoder::GetHandle()
	{
		return m_handle;
	}
	BOOL AACDecoder::Decode(SampleTag& tag, BYTE*& bo, LONG& so)
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
	BOOL AACDecoder::Close()
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

