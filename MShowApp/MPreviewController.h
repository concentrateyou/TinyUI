#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "Control/TinyMenu.h"
using namespace TinyUI;

namespace MShow
{
	class MPreviewView;
	/// <summary>
	/// ‘§¿¿
	/// </summary>
	class MPreviewController : public TinyTaskBase
	{
		friend class MShowController;
		friend class MVideoController;
		friend class MImageController;
		DISALLOW_COPY_AND_ASSIGN(MPreviewController)
	public:
		MPreviewController(MPreviewView& view);
		virtual ~MPreviewController();
		BOOL	Initialize();
		void	SetPulgSize(const TinySize& size);
	public:
		BOOL	Add(DX11Element2D* ps);
		BOOL	Remove(DX11Element2D* ps);
		BOOL	Move(DX11Element2D* ps, BOOL bUp);
		BOOL	Bring(DX11Element2D* ps, BOOL bTop);
	public:
		BOOL	Submit();
		BOOL	Close(DWORD dwMS) OVERRIDE;
	public:
		MPreviewView&	GetView();
		DX11Graphics2D&	Graphics();
	private:
		void	OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMenuClick(void*, INT wID);
		void	OnMessagePump();
		DWORD	Draw();
		DX11Element2D* HitTest(const TinyPoint& pos);
	private:
		BOOL							m_bBreak;
		TinySize						m_pulgSize;
		TinyMenu						m_popup;
		TinyLock						m_lock;
		DX11Element2D*					m_current;
		MPreviewView&					m_view;
		DX11Graphics2D					m_graphics;
		DX11Image2D						m_handles[8];
		TinyArray<DX11Element2D*>		m_array;
		TinyScopedPtr<DX11RenderView>	m_renderView;
		vector<HANDLE>					m_waits;
		TinyPerformanceTimer			m_timer;
	private:
		TinyScopedPtr<Delegate<void(void*, INT)>>				   m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonUp;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onRButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseMove;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseLeave;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSetCursor;
	};
}



