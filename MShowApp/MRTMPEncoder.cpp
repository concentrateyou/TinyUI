#include "stdafx.h"
#include "MRTMPEncoder.h"
#include "MShowApp.h"

namespace MShow
{
	MRTMPEncoder::MRTMPEncoder()
		:m_audioRate(128),
		m_videoFPS(25),
		m_videoRate(1000)
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

	BOOL MRTMPEncoder::Open()
	{
		MVideoController* pCTRL = MShowApp::Instance().GetController().GetVideoController(0);
		if (!pCTRL)
			return FALSE;
		this->Close();
		if (!m_aac.Open(*pCTRL->m_player.GetFormat()))
			return FALSE;
		TinySize size = pCTRL->m_player.GetSize();
		if (!m_x264.Open(size.cx, size.cy, 25, 1000))
			return FALSE;
		return TRUE;
	}

	BOOL MRTMPEncoder::Close()
	{
		m_aac.Close();
		m_x264.Close();
		return TRUE;
	}
}
