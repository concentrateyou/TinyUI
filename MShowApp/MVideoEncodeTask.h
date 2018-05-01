#pragma once
#include "MShowCommon.h"
#include "MShadowView.h"
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
	class MVideoEncodeTask : public TinyThread
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoEncodeTask)
	public:
		MVideoEncodeTask(MRTMPPusher& pusher, MClock& clock);
		virtual ~MVideoEncodeTask();
	public:
		BOOL			Submit(const TinySize& pulgSize, INT videoFPS, INT videoRate);
		BOOL			Close(DWORD dwMS) OVERRIDE;
		TinySize		GetSize() const;
		INT				GetVideoFPS() const;
		INT				GetVideoRate() const;
		QSVEncoder&		GetQSV();
		MSampleQueue&	GetSamples();
	private:
		void			OnMessagePump();
	private:
		MRTMPPusher&			m_pusher;
		BOOL					m_bBreak;
		INT64					m_videoINC;
		INT						m_videoFPS;
		INT						m_videoRate;
		TinySize				m_pulgSize;
		MClock&					m_clock;
		QSVEncoder				m_encoder;
		MSampleQueue			m_samples;
		MPacketQueue			m_queue;
		TinyPerformanceTime	m_timeQPC;
	};
}



