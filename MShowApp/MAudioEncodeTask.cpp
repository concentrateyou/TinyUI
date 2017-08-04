#include "stdafx.h"
#include "MAudioEncodeTask.h"
#include "AudioMixer.h"
#include "MShowApp.h"
#include "MShowController.h"
#include "MVideoController.h"
#include "MAudioController.h"
#include "MPreviewController.h"
#include "Media/TinyWASAPIAudio.h"

namespace MShow
{
	MAudioEncodeTask::MAudioEncodeTask(MRTMPPusher& pusher)
		:m_pusher(pusher),
		m_bBreak(FALSE),
		m_pVideoCTRL(NULL),
		m_pAudioCTRL(NULL)
	{
		m_onAudio.Reset(new Delegate<void(BYTE*, LONG)>(this, &MAudioEncodeTask::OnAudioPCM));
		m_onEffectAudio.Reset(new Delegate<void(BYTE*, LONG)>(this, &MAudioEncodeTask::OnAudioTrack));
	}

	MAudioEncodeTask::~MAudioEncodeTask()
	{
	}

	BOOL MAudioEncodeTask::Submit(INT audioRate)
	{
		ZeroMemory(&m_waveFMT, sizeof(m_waveFMT));
		m_waveFMT.cbSize = 0;
		m_waveFMT.nBlockAlign = 4;
		m_waveFMT.nChannels = 2;
		m_waveFMT.nSamplesPerSec = 44100;
		m_waveFMT.wBitsPerSample = 16;
		m_waveFMT.nAvgBytesPerSec = 176400;
		m_waveFMT.wFormatTag = WAVE_FORMAT_PCM;
		if (!m_aac.Open(m_waveFMT, audioRate))
			return FALSE;
		return TinyTaskBase::Submit(BindCallback(&MAudioEncodeTask::OnMessagePump, this));
	}

	void MAudioEncodeTask::OnMessagePump()
	{
		SampleTag sampleTag;
		for (;;)
		{
			if (m_bBreak)
				break;
			Sleep(25);
			/*ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_queue.Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				continue;
			}
			if (MShowApp::Instance().GetController().GetBaseTime() != -1)
			{
				m_aac.Encode(sampleTag.bits, sampleTag.size);
			}
			SAFE_DELETE_ARRAY(sampleTag.bits);*/
		}
	}

	BOOL MAudioEncodeTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_aac.Close();
			return TRUE;
		}
		return FALSE;
	}

	void MAudioEncodeTask::OnAudioPCM(BYTE* bits, LONG size)
	{
		TinyScopedArray<BYTE> _array(new BYTE[size]);
		if (m_queue.GetSize() > 0)
		{
			SampleTag sampleTag = { 0 };
			if (m_queue.Pop(sampleTag))
			{
				AudioMixer::Mix(bits, sampleTag.bits, size, _array);
			}
		}
		else
		{
			memcpy_s(_array, size, bits, size);
		}
		BYTE* bo = NULL;
		LONG  so = 0;
		Sample sample;
		ZeroMemory(&sample, sizeof(sample));
		if (m_aac.Encode(_array, size, bo, so, sample.mediaTag))
		{
			sample.size = so;
			sample.bits = new BYTE[so];
			memcpy(sample.bits, bo, so);
		}
	}

	void MAudioEncodeTask::OnAudioTrack(BYTE* bits, LONG size)
	{
		SampleTag sampleTag;
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		sampleTag.bits = new BYTE[size];
		sampleTag.size = size;
		memcpy_s(sampleTag.bits, size, bits, size);
		m_queue.Push(sampleTag);
	}


	INT	MAudioEncodeTask::GetAudioRate() const
	{
		return m_audioRate;
	}

	WAVEFORMATEX MAudioEncodeTask::GetFormat() const
	{
		return m_waveFMT;
	}

	AACEncode&	MAudioEncodeTask::GetAAC()
	{
		return m_aac;
	}

	void MAudioEncodeTask::SetVideoController(MVideoController* pCTRL)
	{
		if (m_pVideoCTRL != pCTRL)
		{
			if (pCTRL != NULL)
			{
				pCTRL->AddElement();
				pCTRL->EVENT_AUDIO += m_onAudio;
			}
			Sleep(1);
			if (m_pVideoCTRL != NULL)
			{
				m_pVideoCTRL->EVENT_AUDIO -= m_onAudio;
			}
			m_pVideoCTRL = pCTRL;
		}
	}
	void MAudioEncodeTask::SetAudioController(MAudioController* pCTRL)
	{
		if (m_pAudioCTRL != pCTRL)
		{
			if (pCTRL != NULL)
			{
				pCTRL->EVENT_AUDIO += m_onEffectAudio;
			}
			Sleep(1);
			if (m_pAudioCTRL != NULL)
			{
				m_pAudioCTRL->EVENT_AUDIO -= m_onEffectAudio;
			}
			m_pAudioCTRL = pCTRL;
			m_queue.RemoveAll();
		}
	}
}