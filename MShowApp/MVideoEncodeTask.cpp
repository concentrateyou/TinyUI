#include "stdafx.h"
#include "MVideoEncodeTask.h"
#include "MShowApp.h"

namespace MShow
{
	MVideoEncodeTask::MVideoEncodeTask(MRTMPPusher& pusher, MClock& clock)
		:m_bBreak(FALSE),
		m_pusher(pusher),
		m_videoINC(0),
		m_clock(clock)
	{
	}


	MVideoEncodeTask::~MVideoEncodeTask()
	{
	}
	BOOL MVideoEncodeTask::Submit(const TinySize& pulgSize, INT videoFPS, INT videoRate)
	{
		m_pulgSize = pulgSize;
		m_videoRate = videoRate;
		m_videoFPS = videoFPS;
		if (!m_encoder.Open(m_pulgSize, m_pulgSize, m_videoRate, m_videoFPS))
			return FALSE;
		return TinyTaskBase::Submit(BindCallback(&MVideoEncodeTask::OnMessagePump, this));
	}

	void MVideoEncodeTask::OnMessagePump()
	{
		Sample sample;
		for (;;)
		{
			if (m_bBreak)
				break;
			if (m_clock.GetBaseTime() == -1)
				continue;
			MShadowController* pCTRL = MShow::MShowApp::Instance().GetController().GetShadowController();
			if (pCTRL != NULL)
			{
				SampleTag sampleTag;
				ZeroMemory(&sampleTag, sizeof(sampleTag));
				if (pCTRL->GetVideoQueue().Pop(sampleTag))
				{
					sampleTag.sampleDTS = sampleTag.samplePTS = (m_videoINC++) * 90000 / m_videoFPS;
					ZeroMemory(&sample, sizeof(sample));
					BYTE* bo = NULL;
					LONG  so = 0;
					ZeroMemory(&sample, sizeof(sample));
					if (m_encoder.Encode(sampleTag, bo, so, sample.mediaTag))
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

	BOOL MVideoEncodeTask::Close(DWORD dwMS)
	{
		m_bBreak = TRUE;
		if (TinyTaskBase::Close(dwMS))
		{
			m_encoder.Close();
			m_samples.RemoveAll();
			return TRUE;
		}
		return FALSE;
	}
	TinySize MVideoEncodeTask::GetSize() const
	{
		return m_pulgSize;
	}
	INT MVideoEncodeTask::GetVideoFPS() const
	{
		return m_videoFPS;
	}
	INT MVideoEncodeTask::GetVideoRate() const
	{
		return m_videoRate;
	}
	QSVEncoder&	MVideoEncodeTask::GetQSV()
	{
		return m_encoder;
	}
	MSampleQueue& MVideoEncodeTask::GetSamples()
	{
		return m_samples;
	}
}