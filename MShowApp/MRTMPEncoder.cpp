#include "stdafx.h"
#include "MRTMPEncoder.h"
#include "MShowApp.h"
#include "MShowController.h"
#include "MVideoController.h"

namespace MShow
{
	MRTMPEncoder::MRTMPEncoder()
		:m_audioRate(128),
		m_videoFPS(25),
		m_videoRate(1000),
		m_videoSize(0),
		m_baseTime(timeGetTime()),
		m_x264(BindCallback(&MRTMPEncoder::OnX264, this)),
		m_aac(BindCallback(&MRTMPEncoder::OnAAC, this))
	{
		m_waveFMT.cbSize = 0;
		m_waveFMT.nSamplesPerSec = 44100;
		m_waveFMT.nChannels = 2;
		m_waveFMT.wBitsPerSample = 16;
		m_waveFMT.nBlockAlign = 4;
		m_waveFMT.nAvgBytesPerSec = 176400;
		m_waveFMT.wFormatTag = WAVE_FORMAT_PCM;
	}

	MRTMPEncoder::~MRTMPEncoder()
	{

	}

	BOOL MRTMPEncoder::Initialize(MPreviewView& view)
	{
		TinyRectangle s;
		view.GetClientRect(&s);
		if (!m_dx11.Initialize(view.Handle(), s.Size().cx, s.Size().cy))
			return FALSE;
		return TRUE;
	}

	void MRTMPEncoder::SetAudioConfig(const WAVEFORMATEX& waveFMT, INT audioRate)
	{
		m_waveFMT = waveFMT;
		m_audioRate = audioRate;
	}

	void MRTMPEncoder::SetVideoConfig(const TinySize& pulgSize, INT videoFPS, INT videoRate)
	{
		m_pulgSize = pulgSize;
		m_videoFPS = videoFPS;
		m_videoRate = videoRate;
	}

	INT	MRTMPEncoder::GetVideoFPS() const
	{
		return m_videoFPS;
	}
	INT	MRTMPEncoder::GetVideoRate() const
	{
		return m_videoRate;
	}
	INT	MRTMPEncoder::GetAudioRate() const
	{
		return m_audioRate;
	}

	WAVEFORMATEX MRTMPEncoder::GetFormat() const
	{
		return m_waveFMT;
	}

	TinySize MRTMPEncoder::GetSize() const
	{
		return m_pulgSize;
	}

	AACEncode&	MRTMPEncoder::GetAAC()
	{
		return m_aac;
	}

	x264Encode&	MRTMPEncoder::GetX264()
	{
		return m_x264;
	}

	BOOL MRTMPEncoder::Open()
	{
		MPreviewController* pCTRL = MShowApp::Instance().GetController().GetPreviewController();
		if (!pCTRL)
			return FALSE;
		m_converter.Reset(new I420Converter(m_pulgSize, m_pulgSize));
		if (!m_converter)
			return FALSE;
		if (!m_aac.Open(m_waveFMT))
			return FALSE;
		if (!m_x264.Open(m_pulgSize.cx, m_pulgSize.cy, 25, 1000))
			return FALSE;
		if (!m_image2D.Load(m_dx11, pCTRL->GetRenderView().GetHandle()))
			return FALSE;
		if (!m_copy2D.Create(m_dx11, m_pulgSize.cx, m_pulgSize.cy, NULL, TRUE))
			return FALSE;
		m_bBreaks = FALSE;
		MVideoController* pCTRL1 = MShowApp::Instance().GetController().GetVideoController(0);
		if (!pCTRL1)
			return FALSE;
		m_onAudio.Reset(new Delegate<void(BYTE*, LONG)>(this, &MRTMPEncoder::OnAudio));
		pCTRL1->EVENT_AUDIO += m_onAudio;
		m_videoTask.Submit(BindCallback(&MRTMPEncoder::OnVideoPump, this));
		return TRUE;
	}

	BOOL MRTMPEncoder::Close()
	{
		MVideoController* pCTRL1 = MShowApp::Instance().GetController().GetVideoController(0);
		if (pCTRL1)
		{
			pCTRL1->EVENT_AUDIO -= m_onAudio;
		}
		m_bBreaks = TRUE;
		BOOL bRes = TRUE;
		if (m_videoTask.IsValid())
		{
			bRes &= m_videoTask.Close(INFINITE);
		}
		m_image2D.Destory();
		m_copy2D.Destory();
		m_aac.Close();
		m_x264.Close();
		return bRes;
	}

	void MRTMPEncoder::OnAudio(BYTE* bits, LONG size)
	{
		m_aac.Encode(bits, size);
	}

	void MRTMPEncoder::OnAAC(BYTE* bits, LONG size, const MediaTag& tag)
	{
		Sample sample;
		memcpy(&sample.mediaTag, &tag, sizeof(tag));
		sample.size = size;
		sample.bits = new BYTE[size];
		memcpy(sample.bits, bits, size);
		sample.mediaTag.dwTime = timeGetTime() - m_baseTime + sample.mediaTag.PTS;
		MShowApp::Instance().GetController().GetPusher().m_samples.push(sample);
	}
	void MRTMPEncoder::OnX264(BYTE* bits, LONG size, const MediaTag& tag)
	{
		Sample sample;
		memcpy(&sample.mediaTag, &tag, sizeof(tag));
		sample.size = size;
		sample.bits = new BYTE[size];
		memcpy(sample.bits, bits, size);
		sample.mediaTag.dwTime = timeGetTime() - m_baseTime + sample.mediaTag.PTS;
		MShowApp::Instance().GetController().GetPusher().m_samples.push(sample);
	}

	void MRTMPEncoder::OnVideoPump()
	{
		TinyPerformanceTimer	time;
		DWORD dwMS = static_cast<DWORD>(1000 / m_videoFPS);
		for (;;)
		{
			if (m_bBreaks)
				break;
			time.BeginTime();
			m_copy2D.Copy(m_dx11, m_image2D);
			BYTE* bits = NULL;
			UINT pitch = 0;
			if (m_copy2D.Map(m_dx11, bits, pitch, TRUE))
			{
				DWORD dwSize = pitch * m_pulgSize.cy;
				if (m_videoSize != dwSize && bits != NULL)
				{
					m_videoSize = dwSize;
					m_videoBits.Reset(new BYTE[m_videoSize]);
				}
				memcpy_s(m_videoBits, m_videoSize, bits, dwSize);
				m_copy2D.Unmap(m_dx11);
			}
			if (m_videoBits != NULL && m_converter->BRGAToI420(m_videoBits))
			{
				m_x264.Encode(m_converter->GetI420());
			}
			time.EndTime();
			INT delay = dwMS - static_cast<DWORD>(time.GetMillisconds());
			Sleep(delay < 0 ? 0 : delay);
		}
	}
}
