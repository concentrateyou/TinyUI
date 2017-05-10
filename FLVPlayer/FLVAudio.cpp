#include "stdafx.h"
#include "FLVAudio.h"

namespace FLVPlayer
{
	FLVAudio::FLVAudio(HWND hWND)
		:m_hWND(hWND),
		m_bInitialize(FALSE)
	{
		m_aac.Reset(new AACDecode());
		m_close.CreateEvent();
	}


	FLVAudio::~FLVAudio()
	{
	}
	BOOL FLVAudio::Submit()
	{
		if (m_reader.Open("D:\\4.flv"))
		{
			return TinyTaskBase::Submit(BindCallback(&FLVAudio::OnMessagePump, this));
		}
		return FALSE;
	}
	BOOL FLVAudio::Close(DWORD dwMs)
	{
		m_close.SetEvent();
		return TRUE;
	}

	void FLVAudio::OnMessagePump()
	{
		for (;;)
		{
			if (m_close.Lock(0))
			{
				break;
			}
			FLV_BLOCK block = { 0 };
			if (!m_reader.ReadBlock(block))
			{
				break;
			}
			if (block.type != FLV_AUDIO)
			{
				SAFE_DELETE_ARRAY(block.audio.data);
				SAFE_DELETE_ARRAY(block.video.data);
				continue;
			}
			else
			{

				if (block.audio.codeID == FLV_CODECID_AAC)
				{
					if (block.audio.packetType == FLV_AudioSpecificConfig)
					{
						if (!m_aac->Open(block.audio.data, block.audio.size, block.audio.bitsPerSample == 0 ? 8 : 16))
							break;
					}
					if (block.audio.packetType == FLV_AACRaw)
					{
						BYTE* bo = NULL;
						LONG  so = 0;
						if (m_aac->Decode(block.audio.data, block.audio.size, bo, so))
						{
							if (!m_bInitialize)
							{
								m_bInitialize = TRUE;
								if (!m_player.Initialize(m_hWND, &m_aac->GetFormat(), so * 2))
									break;
								m_event[0].CreateEvent();
								m_event[1].CreateEvent();
								DSBPOSITIONNOTIFY vals[2];
								vals[0].dwOffset = so - 1;
								vals[0].hEventNotify = m_event[0];
								vals[1].dwOffset = so * 2 - 1;
								vals[1].hEventNotify = m_event[1];
								if (m_player.SetNotificationPositions(2, vals))
								{
									m_player.Play();
								}
							}
							m_player.Fill(bo, so);
						}

						HANDLE handles[2] = { m_event[0],m_event[1] };
						HRESULT hRes = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
						if (hRes >= WAIT_OBJECT_0 && hRes <= (WAIT_OBJECT_0 + 1))
						{
							continue;
						}
					}
				}
				SAFE_DELETE_ARRAY(block.audio.data);
				SAFE_DELETE_ARRAY(block.video.data);
			}
		}
		m_reader.Close();
	}
}
