#include "stdafx.h"
#include "AACEncode.h"

namespace Encode
{
	AACEncode::AACEncode()
		:m_inputSamples(0),
		m_maxOutputBytes(0),
		m_timestamp(0),
		m_fPTS(0),
		m_dwINC(0)
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

	BOOL AACEncode::Open(const WAVEFORMATEX& waveFMT, INT audioRate)
	{
		Close();
		m_waveFMT = waveFMT;
		m_aac = faacEncOpen(waveFMT.nSamplesPerSec, waveFMT.nChannels, &m_inputSamples, &m_maxOutputBytes);
		if (!m_aac)
			return FALSE;
		//AAC固定1024
		m_fPTS = AAC_TIMEBASE * AAC_TIMEDEN / static_cast<FLOAT>(waveFMT.nSamplesPerSec);//播放一帧时间
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
	BOOL AACEncode::Encode(BYTE* bi, LONG si, BYTE*& bo, LONG& so, MediaTag& tag)
	{
		if (!m_aac || !bi || si == 0)
			return FALSE;
		INT s = faacEncEncode(m_aac, (int32_t*)bi, m_inputSamples, m_bits, m_maxOutputBytes);
		if (s > 0)
		{
			m_dwINC++;
			m_timestamp += m_fPTS;
			ZeroMemory(&tag, sizeof(tag));
			tag.PTS = static_cast<LONGLONG>(m_fPTS);
			tag.DTS = tag.PTS;
			tag.INC = m_dwINC;
			tag.dwType = 1;
			tag.dwTime = static_cast<DWORD>(m_timestamp);
			tag.dwFlag = 0;
			bo = m_bits;
			so = s;
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
			m_dwINC = 0;
		}
	}
}