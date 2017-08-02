#include "stdafx.h"
#include "MAudioEncodeTask.h"
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
		m_aac(BindCallback(&MAudioEncodeTask::OnAAC, this)),
		m_bBreak(FALSE),
		m_pVideoCTRL(NULL),
		m_pAudioCTRL(NULL)
	{
		m_onAudio.Reset(new Delegate<void(BYTE*, LONG)>(this, &MAudioEncodeTask::OnAudio));
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
			ZeroMemory(&sampleTag, sizeof(sampleTag));
			BOOL bRes = m_queue.Pop(sampleTag);
			if (!bRes || sampleTag.size <= 0)
			{
				Sleep(15);
				continue;
			}
			m_aac.Encode(sampleTag.bits, sampleTag.size);
			SAFE_DELETE_ARRAY(sampleTag.bits);
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

	void MAudioEncodeTask::OnAudio(BYTE* bits, LONG size)
	{
		SampleTag sampleTag;
		ZeroMemory(&sampleTag, sizeof(sampleTag));
		sampleTag.bits = new BYTE[size];
		sampleTag.size = size;
		memcpy_s(sampleTag.bits, size, bits, size);
		m_queue.Push(sampleTag);
	}

	void MAudioEncodeTask::OnAAC(BYTE* bits, LONG size, const MediaTag& tag)
	{
		if (tag.INC == 1)
		{
			if (MShowApp::Instance().GetController().GetBaseTime() == -1)
			{
				MShowApp::Instance().GetController().SetBaseTime(0);
			}
		}
		if (MShowApp::Instance().GetController().GetBaseTime() == 0)
		{
			Sample sample;
			memcpy(&sample.mediaTag, &tag, sizeof(tag));
			sample.size = size;
			sample.bits = new BYTE[size];
			memcpy(sample.bits, bits, size);
			sample.mediaTag.dwTime = static_cast<DWORD>(tag.INC * sample.mediaTag.PTS);
			m_pusher.Publish(sample);
		}
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
			if (m_pAudioCTRL != NULL)
			{
				m_pAudioCTRL->EVENT_AUDIO -= m_onAudio;
			}
			if (pCTRL != NULL)
			{
				pCTRL->EVENT_AUDIO += m_onAudio;
			}
			m_pAudioCTRL = pCTRL;
		}
	}
}