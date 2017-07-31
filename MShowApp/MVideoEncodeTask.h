#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "DXFramework.h"
#include "QSVEncoder.h"
#include "MRTMPPusher.h"
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
		MVideoEncodeTask(MRTMPPusher& pusher);
		virtual ~MVideoEncodeTask();
	public:
		BOOL			Initialize(MPreviewView& view);
		BOOL			Submit(const TinySize& pulgSize, INT videoFPS, INT videoRate);
		BOOL			Close(DWORD dwMS) OVERRIDE;
		TinySize		GetSize() const;
		INT				GetVideoFPS() const;
		INT				GetVideoRate() const;
		QSVEncoder&		GetQSV();
	private:
		void			OnMessagePump();
	private:
		LONGLONG				m_videoINC;
		MRTMPPusher&			m_pusher;
		BOOL					m_bBreak;
		INT						m_videoFPS;
		INT						m_videoRate;
		TinySize				m_pulgSize;
		DX11					m_dx11;
		DX11Texture2D			m_image2D;
		DX11Texture2D			m_copy2D;
		QSVEncoder				m_encoder;
		LONG					m_videoSize;
		TinyScopedArray<BYTE>	m_videoBits;
	};
}



