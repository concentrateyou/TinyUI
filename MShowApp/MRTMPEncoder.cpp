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

	void MRTMPEncoder::SetAudioConfig(const WAVEFORMATEX& waveFMT, INT audioRate)
	{
		m_waveFMT = waveFMT;
		m_audioRate = audioRate;
	}

	void MRTMPEncoder::SetVideoConfig(const TinySize& size, INT videoFPS, INT videoRate)
	{
		m_size = size;
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
		return m_size;
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
		MVideoController* pCTRL = MShowApp::Instance().GetController().GetVideoController(0);
		if (!pCTRL)
			return FALSE;
		MPreviewController* pCTRL1 = MShowApp::Instance().GetController().GetPreviewController();
		if (!pCTRL1)
			return FALSE;
		this->Close();
		if (!m_aac.Open(*pCTRL->m_player.GetFormat()))
			return FALSE;
		TinySize pulgSize = pCTRL1->GetPulgSize();
		if (!m_x264.Open(pulgSize.cx, pulgSize.cy, 25, 1000))
			return FALSE;
		m_converter.Reset(new I420Converter(pulgSize, pulgSize));
		m_bBreaks = FALSE;
		m_videoTask.Submit(BindCallback(&MRTMPEncoder::OnMessagePump, this));
		return TRUE;
	}

	BOOL MRTMPEncoder::Close()
	{
		m_bBreaks = TRUE;
		m_videoTask.Close(INFINITE);
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
		MPreviewController* pCTRL = MShowApp::Instance().GetController().GetPreviewController();
		pCTRL->m_iCopy = 0;
		pCTRL->m_render.SetEvent();
		for (;;)
		{
			if (m_bBreaks)
				break;
			INT delay = dwMS - time;
			Sleep(delay < 0 ? 0 : delay);
			m_time.BeginTime();
			if (pCTRL->m_copy.Lock(0))//Copy
			{
				DX11RenderView* pView = pCTRL->m_views[pCTRL->m_iCopy];
				if (pView != NULL)
				{
					DWORD dwSize = 0;
					BYTE* bits = pView->Map(dwSize);
					if (m_dwSize != dwSize && bits != NULL)
					{
						m_dwSize = dwSize;
						m_bits.Reset(new BYTE[m_dwSize]);
					}
					memcpy_s(m_bits, m_dwSize, bits, dwSize);
					pView->Unmap();
					/*if (m_bits != NULL && m_converter->BRGAToI420(m_bits))
					{
						m_x264.Encode(m_converter->GetI420());
					}*/
				}
				pCTRL->m_render.SetEvent();
			}
			m_time.EndTime();
			time = m_time.GetMillisconds();
		}
	}
}
