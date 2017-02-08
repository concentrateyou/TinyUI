#include "stdafx.h"
#include "AACDecode.h"

namespace Decode
{
	AACDecode::AACDecode()
		:m_handle(NULL),
		m_dwINC(0)
	{
	}

	AACDecode::~AACDecode()
	{
	}

	BOOL AACDecode::Open(WORD wBitsPerSample, WORD wSampleRate)
	{
		m_handle = faacDecOpen();
		if (!m_handle)
			return FALSE;
		faacDecConfiguration* config = faacDecGetCurrentConfiguration(m_handle);
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
		config->useOldADTSFormat = 0;//defined 56 bit ADTS header (value: 0) or the 58 bit ADTS header (value: 1) 
		if (!faacDecSetConfiguration(m_handle, config))
			return FALSE;
		return TRUE;
	}
	BOOL AACDecode::Decode(BYTE* bits, LONG size, DWORD& dwINC)
	{
		ASSERT(m_handle);
		BYTE* pPCM = reinterpret_cast<BYTE*>(NeAACDecDecode(m_handle, &m_frame, bits, size));
		if (m_frame.error > 0 || !pPCM || m_frame.samples <= 0)
			return FALSE;
		LONG o = m_frame.samples * m_frame.channels;
		MediaTag tag;
		tag.dwPTS = 0;
		tag.dwDTS = 0;
		tag.dwType = 1;
		tag.dwTime = timeGetTime();
		tag.dwFlag = 0;
		tag.dwINC = ++m_dwINC;
		OnDone(pPCM, o, tag);
		return TRUE;
	}
	BOOL AACDecode::Close()
	{
		if (m_handle != NULL)
		{
			faacDecClose(m_handle);
			m_handle = NULL;
		}
		return TRUE;
	}
	void AACDecode::OnDone(BYTE* bits, LONG size, const MediaTag& tag)
	{
		EVENT_DONE(bits, size, tag);
	}
}

