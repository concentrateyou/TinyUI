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
		for (;;)
		{
			if (m_bBreak)
				break;
			Sleep(10000);
		}
	}

	BOOL MAudioEncodeTask::Close(DWORD dwMS)
	{
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
			if (MShow::MShowApp::GetInstance().GetController().IsPushing())
			{
				MVideoController* pCTRL = MShowApp::GetInstance().GetController().GetCurrentCTRL();
				if (pCTRL != NULL)
				{
					MPacketAllocQueue& videoQueue = MShow::MShowApp::GetInstance().GetController().GetPreviewController()->GetVideoQueue();
					if (videoQueue.GetCount() <= 15)
					{
						TinyScopedArray<BYTE> output(new BYTE[size]);
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
						Sample sample;
						BYTE* bo = NULL;
						LONG  so = 0;
						ZeroMemory(&sample, sizeof(sample));
						if (m_aac.Encode(sampleTag.bits, sampleTag.size, bo, so, sample.mediaTag))
						{
							sample.size = so;
							sample.bits = new BYTE[so];
							memcpy(sample.bits, bo, so);
							m_samples.Push(sample);
						}
					}
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
			if (m_pVideoCTRL != NULL)
			{
				m_pVideoCTRL->EVENT_AUDIO -= m_onAudio;
			}
			if (pCTRL != NULL)
			{
				pCTRL->AddElement();
				pCTRL->EVENT_AUDIO += m_onAudio;
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