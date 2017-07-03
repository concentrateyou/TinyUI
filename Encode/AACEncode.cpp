#include "stdafx.h"
#include "AACEncode.h"

namespace Encode
{
	AACEncode::AACEncode(Callback<void(BYTE*, LONG, const MediaTag&)>&& callback)
		:m_inputSamples(0),
		m_maxOutputBytes(0),
		m_dwPTS(0),
		m_callback(std::move(callback))
	{
	}

	AACEncode::~AACEncode()
	{

	}

	BOOL AACEncode::GetSpecificInfo(vector<BYTE>& info)
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

	DWORD AACEncode::GetOutputBytes() const
	{
		return m_maxOutputBytes;
	}

	DWORD AACEncode::GetFPS() const
	{
		return m_dwPTS;
	}

	BOOL AACEncode::Open(const WAVEFORMATEX& waveFMT, INT audioRate)
	{
		Close();
		m_waveFMT = waveFMT;
		m_aac = faacEncOpen(waveFMT.nSamplesPerSec, waveFMT.nChannels, &m_inputSamples, &m_maxOutputBytes);
		if (!m_aac)
			return FALSE;
		//AAC固定1024
		m_dwPTS = AAC_TIMEBASE * AAC_TIMEDEN / waveFMT.nSamplesPerSec;//播放一帧时间
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
		m_config->bitRate = audioRate * 1000 / waveFMT.nChannels;//比特位
		INT iRes = faacEncSetConfiguration(m_aac, m_config);
		if (!iRes)
			return FALSE;
		m_bits.Reset(new BYTE[m_maxOutputBytes]);
		return TRUE;
	}
	BOOL AACEncode::Encode(BYTE* bits, LONG size)
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
	void AACEncode::Close()
	{
		if (m_aac != NULL)
		{
			faacEncClose(m_aac);
			m_aac = NULL;
		}
	}
}