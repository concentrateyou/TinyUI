#pragma once
#include "Common/TinyTime.h"
#include "MShowCommon.h"
#include "MPreviewController.h"
#include "MRTMPClient.h"

namespace MShow
{
	/// <summary>
	/// RTMP±àÂëÆ÷
	/// </summary>
	class MRTMPEncoder : public TinyTaskBase
	{
	public:
		MRTMPEncoder(MPreviewController& controller);
		virtual ~MRTMPEncoder();
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL				m_bClose;
		MPreviewController&	m_controller;
	};
}

