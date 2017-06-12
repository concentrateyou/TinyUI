#pragma once
#include "MShowCommon.h"
#include "MFLVTask.h"
#include "DXView.h"

namespace MShow
{
	class DXView;

	class MShowController
	{
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(DXView& view);
		virtual ~MShowController();
		BOOL Initialize(LPCSTR pzURL);
		void Uninitialize();
	private:
		void OnVideo(ID2D1Bitmap1*);
	private:
		DX2D		m_d2d;
		MFLVTask	m_task;
		DXView&		m_view;
		TinyScopedPtr<Delegate<void(ID2D1Bitmap1*)>> m_onVideo;
	};
}



