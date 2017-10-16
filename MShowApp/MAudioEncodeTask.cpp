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
	MAudioEncodeTask::MAudioEncodeTask(MRTMPPusher& pusher, MClock& clock)
		:m_pusher(pusher),
		m_bBreak(FALSE),
		m_pVideoCTRL(NULL),
		m_pAudioCTRL(NULL),
		m_clock(clock)
	{
		m_onAudio.Reset(new Delegate<void(BYTE*, LONG)>(this, &MAudioEncodeTask::OnAudio));
		m_onAudioMix.Reset(new Delegate<void(BYTE*, LONG)>(this, &MAudioEncodeTask::OnAudioMix));
		m_event.CreateEvent();
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
		Sample sample;
		SampleTag sampleTag;
		for (;;)
		{
			if (m_bBreak)
				break;
			if (m_event.Lock(INFINITE))
			{
				ZeroMemory(&sampleTag, sizeof(sampleTag));
				BOOL bRes = m_queue.Pop(sampleTag);
				if (bRes && sampleTag.size > 0)
				{
					BYTE* bo = NULL;
					LONG  so = 0;
					ZeroMemory(&sample, sizeof(sample));
					if (m_aac.Encode(sampleTag.bits, sampleTag.size, bo, so, sample.mediaTag))
					{
						//TRACE("Audio timestamp:%lld\n", sampleTag.timestamp);
						//sample.mediaTag.dwTime = sampleTag.timestamp;
						sample.size = so;
						sample.bits = new BYTE[so];
						memcpy(sample.bits, bo, so);
						m_samples.Push(sample);
					}
				}
				SAFE_DELETE_ARRAY(sampleTag.bits);
			}
		}
	}

	BOOL MAudioEncodeTask::Close(DWORD dwMS)
	{
		m_event.SetEvent();
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_aac.Close();
			m_samples.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}

	void MAudioEncodeTask::OnAudio(BYTE* bits, LONG size)
	{
		if (m_clock.GetBaseTime() == -1)
		{
			m_clock.SetBaseTime(MShow::MShowApp::GetInstance().GetQPCTimeMS());
		}
		if (m_clock.GetBaseTime() != -1)
		{
			MPacketAllocQueue& videoQueue = MShow::MShowApp::GetInstance().GetController().GetPreviewController()->GetVideoQueue();
			if (MShow::MShowApp::GetInstance().GetController().IsPushing())
			{
				if (videoQueue.GetCount() <= 5)
				{
					BYTE* output = new BYTE[size];
					if (m_queueMix.GetSize() > 0)
					{
						SampleTag sampleTag = { 0 };
						if (m_queueMix.Pop(sampleTag))
						{
							AudioMixer::Mix(bits, sampleTag.bits, size, output);
						}
					}
					else
					{
						memcpy_s(output, size, bits, size);
					}
					SampleTag sampleTag;
					ZeroMemory(&sampleTag, sizeof(sampleTag));
					sampleTag.bits = output;
					sampleTag.size = size;
					/*m_clock.SetAudioPTS(MShow::MShowApp::GetInstance().GetQPCTimeMS());
					sampleTag.timestamp = m_clock.GetAudioPTS() - m_clock.GetBaseTime();*/
					m_queue.Push(sampleTag);
					m_event.SetEvent();
				}
			}
		}
	}

	void MAudioEncodeTask::OnAudioMix(BYTE* bits, LONG size)
	{
		SampleTag sampleTag;
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		sampleTag.bits = new BYTE[size];
		sampleTag.size = size;
		memcpy_s(sampleTag.bits, size, bits, size);
		m_queueMix.Push(sampleTag);
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
	MSampleQueue& MAudioEncodeTask::GetSamples()
	{
		return m_samples;
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
			m_queueMix.RemoveAll();
			if (pCTRL != NULL)
			{
				pCTRL->EVENT_AUDIO += m_onAudioMix;
			}
			if (m_pAudioCTRL != NULL)
			{
				m_pAudioCTRL->EVENT_AUDIO -= m_onAudioMix;
			}
			m_pAudioCTRL = pCTRL;
		}
	}
}