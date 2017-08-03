#include "stdafx.h"
#include "AACEncoder.h"

namespace Encode
{
	AACEncoder::AACEncoder(Callback<void(BYTE*, LONG, const MediaTag&)>&& callback)
		:m_inputSamples(0),
		m_maxOutputBytes(0),
		m_dwPTS(0),
		m_dwINC(0),
		m_callback(std::move(callback))
	{
	}

	AACEncoder::~AACEncoder()
	{

	}

	BOOL AACEncoder::GetSpecificInfo(vector<BYTE>& info)
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

	DWORD AACEncoder::GetOutputBytes() const
	{
		return m_maxOutputBytes;
	}

	DWORD AACEncoder::GetFPS() const
	{
		return m_dwPTS;
	}

	BOOL AACEncoder::Open(const WAVEFORMATEX& waveFMT, INT audioRate)
	{
		Close();
		m_waveFMT = waveFMT;
		m_aac = faacEncOpen(waveFMT.nSamplesPerSec, waveFMT.nChannels, &m_inputSamples, &m_maxOutputBytes);
		if (!m_aac)
			return FALSE;
		//AAC�̶�1024
		m_dwPTS = AAC_TIMEBASE * AAC_TIMEDEN / waveFMT.nSamplesPerSec;//����һ֡ʱ��
		m_config = faacEncGetCurrentConfiguration(m_aac);
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
		m_config->quantqual = 100;
		m_config->mpegVersion = MPEG4;
		m_config->aacObjectType = LOW;
		m_config->useTns = 1;
		m_config->useLfe = 0;
		m_config->bandWidth = 0;
		m_config->allowMidside = 0;
		m_config->outputFormat = 1;//ADTS
		m_config->shortctl = SHORTCTL_NORMAL;
		m_config->bitRate = audioRate * 1000 / waveFMT.nChannels;//����λ
		INT iRes = faacEncSetConfiguration(m_aac, m_config);
		if (!iRes)
			return FALSE;
		m_bits.Reset(new BYTE[m_maxOutputBytes]);
		return TRUE;
	}
	BOOL AACEncoder::Encode(BYTE* bits, LONG size)
	{
		if (!m_aac || !bits || size == 0)
			return FALSE;
		INT s = faacEncEncode(m_aac, (int32_t*)bits, m_inputSamples, m_bits, m_maxOutputBytes);
		if (s > 0)
		{
			Media::MediaTag tag;
			tag.PTS = m_dwPTS;
			tag.DTS = 0;
			tag.INC = ++m_dwINC;
			tag.dwType = 1;
			tag.dwTime = timeGetTime();
			tag.dwFlag = 0;
			if (!m_callback.IsNull())
			{
				m_callback(m_bits, s, tag);
			}
			return TRUE;
		}
		return FALSE;
	}
	void AACEncoder::Close()
	{
		if (m_aac != NULL)
		{
			faacEncClose(m_aac);
			m_aac = NULL;
			m_dwINC = 0;
		}
	}
}