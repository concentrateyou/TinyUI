#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "DXFramework.h"
#include "QSVEncoder.h"
#include "MPacketQueue.h"
#include "MSampleQueue.h"
#include "MRTMPPusher.h"
#include "MClock.h"
using namespace DXFramework;
using namespace QSV;

namespace MShow
{
	/// <summary>
	///  ”∆µ±‡¬Î
	/// </summary>
	class MVideoEncodeTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoEncodeTask)
	public:
		MVideoEncodeTask(MRTMPPusher& pusher, MClock& clock);
		virtual ~MVideoEncodeTask();
	public:
		BOOL			Initialize(MPreviewView& view);
		BOOL			Submit(const TinySize& pulgSize, INT videoFPS, INT videoRate);
		BOOL			Close(DWORD dwMS) OVERRIDE;
		TinySize		GetSize() const;
		INT				GetVideoFPS() const;
		INT				GetVideoRate() const;
		QSVEncoder&		GetQSV();
		MSampleQueue&	GetSamples();
	private:
		void			OnMessagePump();
		DWORD			Encode(Sample& sample);
	private:
		LONGLONG				m_videoINC;
		MRTMPPusher&			m_pusher;
		BOOL					m_bBreak;
		INT						m_videoFPS;
		INT						m_videoRate;
		LONG					m_videoSize;
		TinyScopedArray<BYTE>	m_videoBits;
		TinySize				m_pulgSize;
		MClock&					m_clock;
		DX11					m_dx11;
		DX11Texture2D			m_image2D;
		DX11Texture2D			m_copy2D;
		QSVEncoder				m_encoder;
		MSampleQueue			m_samples;
		TinyPerformanceTimer	m_timer;
	};
}



