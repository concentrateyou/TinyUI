#include "stdafx.h"
#include "DSAudioOutputStream.h"

namespace MediaSDK
{
	static const UINT	MaxOpenBufferSize = 1024 * 1024 * 64;
	static const INT	MaxChannelsToMask = 8;
	static const UINT	ChannelsToMask[MaxChannelsToMask + 1] =
	{
		0,
		// 1 = Mono
		SPEAKER_FRONT_CENTER,
		// 2 = Stereo
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT,
		// 3 = Stereo + Center
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER,
		// 4 = Quad
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		// 5 = 5.0
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		// 6 = 5.1
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT,
		// 7 = 6.1
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_BACK_CENTER,
		// 8 = 7.1
		SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT
	};

	DSAudioOutputStream::DSAudioOutputStream()
		:m_callback(NULL),
		m_count(0),
		m_pending(0),
		m_offset(0),
		m_size(0),
		m_hWND(NULL),
		m_state(PCM_CLOSED)
	{
		m_event.CreateEvent();
	}


	DSAudioOutputStream::~DSAudioOutputStream()
	{
		m_event.Close();
	}

	void DSAudioOutputStream::OnOpen()
	{

	}

	void DSAudioOutputStream::OnClose()
	{

	}

	void DSAudioOutputStream::HandleError(HRESULT hRes)
	{
		LOG(ERROR) << "DSAudioOutputStream error " << hRes;
		if (m_callback != NULL)
		{
			m_callback->OnError();
		}
	}
	BOOL DSAudioOutputStream::Initialize(const AudioParameters& params, UINT count, HWND hWND, const GUID& dID)
	{
		m_dID = dID;
		m_count = count;
		m_params = params;
		m_hWND = hWND;
		const WAVEFORMATEX* pFMT = m_params.GetFormat();
		m_waveFMT.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		m_waveFMT.Format.nChannels = pFMT->nChannels;
		m_waveFMT.Format.nSamplesPerSec = pFMT->nSamplesPerSec;
		m_waveFMT.Format.wBitsPerSample = pFMT->wBitsPerSample;
		m_waveFMT.Format.cbSize = sizeof(m_waveFMT) - sizeof(WAVEFORMATEX);
		m_waveFMT.Format.nBlockAlign = (m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8;//每个采样字节数(多声道)
		m_waveFMT.Format.nAvgBytesPerSec = m_waveFMT.Format.nBlockAlign * m_waveFMT.Format.nSamplesPerSec;
		m_waveFMT.dwChannelMask = ChannelsToMask[m_waveFMT.Format.nChannels > MaxChannelsToMask ? MaxChannelsToMask : m_waveFMT.Format.nChannels];
		m_waveFMT.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		m_waveFMT.Samples.wValidBitsPerSample = m_waveFMT.Format.wBitsPerSample;
		m_state = PCM_CLOSED;
		m_size = ((m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8) * m_params.GetFrames();//缓冲区大小
		m_packetsize = m_size;
		m_packet.Reset(new AudioPacket(params));
		ASSERT(m_packet);
		return TRUE;
	}

	BOOL DSAudioOutputStream::Open()
	{
		if (m_state != PCM_CLOSED)
			return FALSE;
		if (m_packetsize * m_count > MaxOpenBufferSize)
			return FALSE;
		if (m_count < 2 || m_count > 5)
			return FALSE;
		m_offset = 0;
		::GetLastError();
		m_bits.Reset(new CHAR[m_packetsize * m_count]);
		ASSERT(m_bits);
		ZeroMemory(m_bits, m_packetsize * m_count);

		TinyScopedArray<DSBPOSITIONNOTIFY> notifys(new DSBPOSITIONNOTIFY[m_count]);
		ASSERT(notifys);
		ZeroMemory(notifys, sizeof(DSBPOSITIONNOTIFY) * m_count);
		TinyComPtr<IDirectSoundBuffer>	dsb;
		TinyComPtr<IDirectSoundNotify>	notify;
		DSBCAPS dsbcaps = { 0 };
		dsbcaps.dwSize = sizeof(DSBCAPS);
		DSBUFFERDESC dbdesc;
		ZeroMemory(&dbdesc, sizeof(dbdesc));
		dbdesc.dwSize = sizeof(dbdesc);
		dbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		HRESULT hRes = S_OK;
		hRes = DirectSoundCreate8(&m_dID, &m_sound, NULL);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_sound->SetCooperativeLevel(m_hWND, DSSCL_PRIORITY);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_sound->CreateSoundBuffer(&dbdesc, &m_primary, NULL);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_primary->SetFormat(reinterpret_cast<WAVEFORMATEX*>(&m_waveFMT));
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_primary->GetCaps(&dsbcaps);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		ZeroMemory(&dbdesc, sizeof(dbdesc));
		dbdesc.dwSize = sizeof(dbdesc);
		dbdesc.dwFlags = DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY;
		dbdesc.dwBufferBytes = m_size * m_count;
		dbdesc.lpwfxFormat = reinterpret_cast<WAVEFORMATEX*>(&m_waveFMT);
		hRes = m_sound->CreateSoundBuffer(&dbdesc, &dsb, NULL);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = dsb->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_secondary);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_secondary->QueryInterface(IID_IDirectSoundNotify, (void**)&notify);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		for (UINT32 i = 0; i < m_count; i++)
		{
			notifys[i].dwOffset = (i + 1) * m_size - 1;
			notifys[i].hEventNotify = m_event;
		}
		if (FAILED(notify->SetNotificationPositions(m_count, notifys)))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		m_state = PCM_READY;
		return TRUE;
	_ERROR:
		m_callback = NULL;
		m_state = PCM_CLOSED;
		m_sound.Release();
		m_primary.Release();
		m_secondary.Release();
		return FALSE;
	}
	void DSAudioOutputStream::OnCallback(BOOLEAN timerFired)
	{
		do
		{
			if (m_state != PCM_PLAYING)
				break;
			m_pending -= m_size;
			m_offset += 1;
			m_offset %= m_count;
			CHAR* pval = &m_bits[m_offset * m_size];
			QueuePacket(pval, m_size);
			if (m_state != PCM_PLAYING)
				break;
			FillPacket(m_offset * m_size, pval, m_size);
			m_pending += m_size;
		} while (0);
	}
	BOOL DSAudioOutputStream::FillPacket(UINT32 offset, CHAR* bits, UINT32 size)
	{
		LPVOID	ppvAudioPtr = NULL;
		DWORD	dwAudioBytes = 0;
		HRESULT hRes = m_secondary->Lock(offset, size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
		if (FAILED(hRes))
		{
			if (hRes != DSERR_BUFFERLOST)
			{
				HandleError(hRes);
				return FALSE;
			}
			hRes = m_secondary->Restore();
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
			hRes = m_secondary->Lock(offset, m_size, &ppvAudioPtr, &dwAudioBytes, NULL, 0, 0);
			if (FAILED(hRes))
			{
				HandleError(hRes);
				return FALSE;
			}
		}
		memcpy(ppvAudioPtr, bits, m_size);
		hRes = m_secondary->Unlock(ppvAudioPtr, dwAudioBytes, NULL, 0);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			return FALSE;
		}
		return TRUE;
	}
	void DSAudioOutputStream::QueuePacket(CHAR* bits, UINT32 size)
	{
		const INT64 delay = (m_pending * 1000 * 1000) / m_waveFMT.Format.nAvgBytesPerSec;
		INT32 count = m_callback->OnInput(delay, TinyPerformanceTime::Now(), 0, m_packet);
		size = min(((m_waveFMT.Format.nChannels * m_waveFMT.Format.wBitsPerSample) / 8) * count, size);//读到的字节数
		do
		{
			if (size > m_size)//大于分配的字节数
			{
				HandleError(MMSYSERR_NOERROR);
				break;
			}
			memcpy_s(bits, size, m_packet->data(), size);
		} while (0);
	}

	BOOL DSAudioOutputStream::Start(AudioInputCallback* callback)
	{
		if (m_state != PCM_READY)
			return FALSE;
		HRESULT hRes = S_OK;
		if (!m_event.ResetEvent())
		{
			HandleError(::GetLastError());
			goto _ERROR;
		}
		m_waiter.Unregister();
		if (!m_waiter.Register(m_event, INFINITE, BindCallback(&DSAudioOutputStream::OnCallback, this)))
		{
			HandleError(::GetLastError());
			goto _ERROR;
		}
		m_callback = callback;
		m_state = PCM_PLAYING;
		m_pending = 0;
		//读取外部数据
		for (UINT32 i = 0; i < m_count; ++i)
		{
			CHAR* pval = &m_bits[i * m_size];
			QueuePacket(pval, m_size);
			m_pending += m_size;
		}
		::MemoryBarrier();
		//填充缓冲区
		for (UINT32 i = 0; i < m_count; ++i)
		{
			CHAR* pval = &m_bits[i * m_size];
			FillPacket(i * m_size, pval, m_size);
		}
		hRes = m_secondary->SetCurrentPosition(0);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		hRes = m_secondary->Play(0, 0, DSBPLAY_LOOPING);
		if (FAILED(hRes))
		{
			HandleError(hRes);
			goto _ERROR;
		}
		return TRUE;
	_ERROR:
		m_state = PCM_READY;
		m_callback = NULL;
		return FALSE;
	}

	BOOL DSAudioOutputStream::Stop()
	{
		if (m_state != PCM_PLAYING)
			return FALSE;
		m_state = PCM_STOPPING;
		::MemoryBarrier();
		HRESULT hRes = S_OK;
		if (m_secondary != NULL)
		{
			hRes = m_secondary->Restore();
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
			hRes = m_secondary->Stop();
			if (FAILED(hRes))
			{
				HandleError(hRes);
				goto _ERROR;
			}
		}
	_ERROR:
		m_state = PCM_READY;
		m_callback = NULL;
		return SUCCEEDED(hRes);
	}

	BOOL DSAudioOutputStream::GetVolume(FLOAT* volume)
	{
		HRESULT hRes = S_OK;
		if (m_secondary != NULL)
		{
			LONG val = 0;
			hRes = m_secondary->GetVolume(&val);
			if (FAILED(hRes))
				HandleError(hRes);
			*volume = static_cast<FLOAT>(val);
			return SUCCEEDED(hRes);
		}
		return FALSE;
	}

	BOOL DSAudioOutputStream::SetVolume(FLOAT volume)
	{
		HRESULT hRes = S_OK;
		if (m_secondary != NULL)
		{
			hRes = m_secondary->SetVolume(static_cast<LONG>(volume));
			if (FAILED(hRes))
				HandleError(hRes);
			return SUCCEEDED(hRes);
		}
		return FALSE;
	}
	void DSAudioOutputStream::Close()
	{
		HRESULT hRes = S_OK;
		m_callback = NULL;
		m_event.SetEvent();
		m_event.ResetEvent();
		m_waiter.Unregister();
		if (m_secondary != NULL)
		{
			hRes = m_secondary->Restore();
			if (FAILED(hRes))
				HandleError(hRes);
			hRes = m_secondary->Stop();
			if (FAILED(hRes))
				HandleError(hRes);
		}
		if (m_primary != NULL)
		{
			hRes = m_primary->Restore();
			if (FAILED(hRes))
				HandleError(hRes);
			hRes = m_primary->Stop();
			if (FAILED(hRes))
				HandleError(hRes);
		}
		m_secondary.Release();
		m_primary.Release();
		m_state = PCM_CLOSED;
	}
}