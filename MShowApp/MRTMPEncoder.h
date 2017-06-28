#pragma once
#include "Common/TinyTime.h"
#include "MShowCommon.h"
#include "MPreviewController.h"
#include "MRTMPClient.h"
#include "x264Encode.h"
#include "AACEncode.h"
using namespace Encode;

namespace MShow
{
	/// <summary>
	/// RTMP±àÂëÆ÷
	/// </summary>
	class MRTMPEncoder
	{
	public:
		MRTMPEncoder();
		virtual ~MRTMPEncoder();
		BOOL	Open();
		BOOL	Close();
	public:
		void SetAudioConfig(const WAVEFORMATEX& waveFMT, INT audioRate = 128);
		void SetVideoConfig(const TinySize& size, INT videoFPS = 40, INT videoRate = 1000);
	private:
		INT				m_audioRate;
		INT				m_videoFPS;
		INT				m_videoRate;
		WAVEFORMATEX	m_waveFMT;
		TinySize		m_size;
		x264Encode		m_x264;
		AACEncode		m_aac;
	};
}

