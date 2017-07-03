#pragma once
#include "Common/TinyTime.h"
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "MRTMPClient.h"
#include "x264Encode.h"
#include "AACEncode.h"
#include "I420Converter.h"
#include <concurrent_queue.h>
using namespace Encode;

namespace MShow
{
	/// <summary>
	/// RTMP±àÂëÆ÷
	/// </summary>
	class MRTMPEncoder
	{
		friend class MPreviewController;
		DISALLOW_COPY_AND_ASSIGN(MRTMPEncoder)
	public:
		MRTMPEncoder();
		virtual ~MRTMPEncoder();
		BOOL	Initialize(MPreviewView& view);
		BOOL	Open();
		BOOL	Close();
	public:
		void	SetAudioConfig(const WAVEFORMATEX& waveFMT, INT audioRate = 128);
		void	SetVideoConfig(const TinySize& size, INT videoFPS = 40, INT videoRate = 1000);
		INT		GetVideoFPS() const;
		INT		GetVideoRate() const;
		INT		GetAudioRate() const;
		WAVEFORMATEX	GetFormat() const;
		TinySize		GetSize() const;
		AACEncode&		GetAAC();
		x264Encode&		GetX264();
	private:
		void OnVideoPump();
		void OnAudio(BYTE* bits, LONG size);
		void OnX264(BYTE* bits, LONG size, const MediaTag& tag);
		void OnAAC(BYTE* bits, LONG size, const MediaTag& tag);
	private:
		INT						m_audioRate;
		INT						m_videoFPS;
		INT						m_videoRate;
		DWORD					m_videoSize;
		BOOL					m_bBreaks;
		LONG					m_baseTime;
		WAVEFORMATEX			m_waveFMT;
		TinySize				m_pulgSize;
		x264Encode				m_x264;
		AACEncode				m_aac;
		TinyTaskBase			m_videoTask;
		DX11					m_dx11;
		DX11Texture2D			m_image2D;
		DX11Texture2D			m_copy2D;
		TinyScopedArray<BYTE>	m_videoBits;
		TinyScopedPtr<I420Converter>	m_converter;
		TinyScopedPtr<Delegate<void(BYTE*, LONG)>>	m_onAudio;
	};
}

