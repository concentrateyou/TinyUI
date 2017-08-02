#include "stdafx.h"
#include "MP3Decode.h"

namespace Decode
{
	MP3Decode::MP3Decode()
		:m_handle(NULL)
	{
		mpg123_init();
	}


	MP3Decode::~MP3Decode()
	{
		mpg123_exit();
	}
	BOOL MP3Decode::Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback)
	{
		m_callback = std::move(callback);
		return TRUE;
	}
	BOOL MP3Decode::Open()
	{
		INT iRes = 0;
		m_handle = mpg123_new(NULL, &iRes);
		if (!m_handle)
			return FALSE;
		if (mpg123_param(m_handle, MPG123_VERBOSE, 2, 0) == MPG123_ERR)
			goto MPG123_ERROR;
		if (mpg123_open_feed(m_handle) == MPG123_ERR)
			goto MPG123_ERROR;
		return TRUE;
	MPG123_ERROR:
		mpg123_close(m_handle);
		mpg123_delete(m_handle);
		m_handle = NULL;
		return FALSE;
	}
	BOOL MP3Decode::Decode(BYTE* rawdata, LONG rawsize)
	{
		if (!m_handle)
			return FALSE;
		if (mpg123_feed(m_handle, rawdata, rawsize) != MPG123_OK)
			return FALSE;
		off_t offset = 0;
		for (;;)
		{
			size_t outsize = 0;
			BYTE* outdata = NULL;
			INT iRes = mpg123_decode_frame(m_handle, &offset, &outdata, &outsize);
			if (iRes > MPG123_OK || iRes == MPG123_ERR)
				return FALSE;
			if (iRes == MPG123_NEED_MORE)
			{
				return TRUE;
			}
			if (iRes == MPG123_OK || iRes == MPG123_DONE)
			{
				if (!m_callback.IsNull())
				{
					m_callback(outdata, outsize, reinterpret_cast<LPVOID>(m_waveFMT.Ptr()));
				}
			}
			if (iRes == MPG123_NEW_FORMAT)
			{
				INT  encoding = 0;
				INT  channels = 0;
				LONG samplesPerSec = 0;
				if (mpg123_getformat(m_handle, &samplesPerSec, &channels, &encoding) != MPG123_OK)
					return FALSE;
				if (m_waveFMT.IsEmpty())
				{
					m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
					if (samplesPerSec == 0)
						samplesPerSec = 44100;
					if (channels == 0)
						channels = MPG123_STEREO;
					if (encoding == 0)
						encoding = MPG123_ENC_SIGNED_16;
					if (mpg123_format_none(m_handle) != MPG123_OK)
						return FALSE;
					if (mpg123_format(m_handle, samplesPerSec, channels, encoding) != MPG123_OK)
						return FALSE;
					WAVEFORMATEX* pFMT = reinterpret_cast<WAVEFORMATEX*>(m_waveFMT.Ptr());
					pFMT->cbSize = 0;
					pFMT->wFormatTag = WAVE_FORMAT_PCM;
					pFMT->nChannels = channels;
					pFMT->nSamplesPerSec = samplesPerSec;
					pFMT->wBitsPerSample = mpg123_encsize(encoding) * 8;
					pFMT->nBlockAlign = pFMT->nChannels * (pFMT->wBitsPerSample / 8);
					pFMT->nAvgBytesPerSec = pFMT->nChannels * pFMT->nSamplesPerSec * pFMT->wBitsPerSample / 8;
				}
				if (!m_callback.IsNull())
				{
					m_callback(NULL, 0, reinterpret_cast<LPVOID>(m_waveFMT.Ptr()));
				}
			}
		}
	}
	BOOL MP3Decode::Close()
	{
		if (m_handle != NULL)
		{
			mpg123_close(m_handle);
			mpg123_delete(m_handle);
			m_handle = NULL;
		}
		return TRUE;
	}
	BOOL MP3Decode::SetFormat(WAVEFORMATEX& sFMT)
	{
		if (m_waveFMT.IsEmpty())
		{
			m_waveFMT.Reset(new BYTE[sizeof(WAVEFORMATEX)]);
		}
		memcpy(m_waveFMT, &sFMT, sizeof(WAVEFORMATEX));
		if (m_handle != NULL)
		{
			if (mpg123_format_none(m_handle) != MPG123_OK)
				return FALSE;
			INT encoding = sFMT.wBitsPerSample / 8;
			if (encoding == 1)
				encoding = MPG123_ENC_SIGNED_8;
			if (encoding == 2)
				encoding = MPG123_ENC_SIGNED_16;
			if (encoding == 4)
				encoding = MPG123_ENC_SIGNED_32;
			if (mpg123_format(m_handle, sFMT.wBitsPerSample, sFMT.nChannels, encoding) != MPG123_OK)
				return FALSE;
		}
		return TRUE;
	}
	WAVEFORMATEX* MP3Decode::GetFormat()
	{
		return reinterpret_cast<WAVEFORMATEX*>(&m_waveFMT);
	}
}
