#include "stdafx.h"
#include "MRTMPEncoder.h"
#include "MShowApp.h"
#include "MShowController.h"

namespace MShow
{
	MRTMPEncoder::MRTMPEncoder()
		:m_audioRate(128),
		m_videoFPS(25),
		m_videoRate(1000),
		m_dwSize(0),
		m_x264(BindCallback(&MRTMPEncoder::OnX264, this))
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
		this->Close();
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
		DX11RenderView* renderView = pCTRL->GetRenderView();
		if (!renderView)
			return FALSE;
		if (!m_image2D.Load(m_dx11, renderView->GetHandle()))
			return FALSE;
		if (!m_copy2D.Create(m_dx11, m_pulgSize.cx, m_pulgSize.cy, NULL, FALSE))
			return FALSE;
		m_bBreaks = FALSE;
		return m_videoTask.Submit(BindCallback(&MRTMPEncoder::OnMessagePump, this));
	}

	BOOL MRTMPEncoder::Close()
	{
		m_bBreaks = TRUE;
		if (m_videoTask.IsValid())
			m_videoTask.Close(INFINITE);
		m_image2D.Destory();
		m_copy2D.Destory();
		m_aac.Close();
		m_x264.Close();
		return TRUE;
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

	void MRTMPEncoder::OnMessagePump()
	{
		m_baseTime = timeGetTime();
		LONGLONG time = 0;
		DWORD dwMS = static_cast<DWORD>(1000 / m_videoFPS);
		for (;;)
		{
			if (m_bBreaks)
				break;
			INT delay = dwMS - time;
			Sleep(delay < 0 ? 0 : delay);
			m_time.BeginTime();
			m_copy2D.Copy(m_dx11, m_image2D);
			m_time.EndTime();
			time = m_time.GetMillisconds();
		}
	}
}
