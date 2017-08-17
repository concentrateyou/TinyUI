#pragma once
#include "MShowCommon.h"
#include "MShadowView.h"
#include "MPacketQueue.h"
#include "MClock.h"
using namespace TinyUI;

namespace MShow
{
	class MShadowView;
	/// <summary>
	/// Ó°×ÓUI¿ØÖÆÆ÷
	/// </summary>
	class MShadowController : public TinyTaskBase
	{
		friend class MShowController;
		friend class MVideoController;
		friend class MImageController;
		friend class MRTMPEncoder;
		DISALLOW_COPY_AND_ASSIGN(MShadowController)
	public:
		MShadowController(MShadowView& view,MClock& clock);
		virtual ~MShadowController();
		BOOL		Initialize();
		BOOL		SetPulgSize(const TinySize& size);
		TinySize	GetPulgSize() const;
		void		SetVideoFPS(INT	videoFPS);
		INT			GetVideoFPS() const;
		MClock&		GetClock();
		TinyEvent&	GetSignal();
	public:
		BOOL		Submit();
		BOOL		Close(DWORD dwMS) OVERRIDE;
	private:
		void		OnMessagePump();
		DWORD		OnVideo(SampleTag& sampleTag);
	public:
		MShadowView&		GetView();
		MPacketAllocQueue&	GetVideoQueue();
	private:
		BOOL					m_bBreak;
		INT						m_videoFPS;
		TinySize				m_pulgSize;
		DX11					m_dx11;
		DX11Texture2D			m_image2D;
		DX11Texture2D			m_copy2D;
		MShadowView&			m_view;
		MPacketQueue			m_queue;
		MPacketAllocQueue		m_videoQueue;
		TinyPerformanceTimer	m_timeQPC;
		MClock&					m_clock;
		TinyEvent				m_signal;
	};
}


