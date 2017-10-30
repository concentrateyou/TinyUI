#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "MFLVPlayer.h"
#include "Control/TinyMenu.h"
#include "DX11Graphics2D.h"
#include "DX11Image2D.h"
using namespace DXFramework;
using namespace TinyUI;

namespace MShow
{
	class MPreviewView;
	/// <summary>
	/// Ԥ��
	/// </summary>
	class MPreviewController
	{
		friend class MShowController;
		DISALLOW_COPY_AND_ASSIGN(MPreviewController)
	public:
		MPreviewController(MPreviewView& view);
		virtual ~MPreviewController();
		BOOL			Initialize();
		BOOL			Open(LPCSTR pzURL);
		BOOL			Close();
		MPreviewView&	GetView();
		LONGLONG		GetBasePTS();
	private:
		void			OnAudio(BYTE* bits, LONG size);
		void			OnVideoCopy(BYTE* bits, LONG size);
		void			OnVideoRender();
	private:
		TinySize					m_videoSize;
		TinySize					m_viewSize;
		TinyPerformanceTime		m_timeQPC;
		DX11Graphics2D				m_graphics;
		DX11Image2D					m_video2D;
		MPreviewView&				m_view;
		TinyScopedPtr<MFLVPlayer>	m_player;
	};
}



