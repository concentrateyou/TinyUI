#pragma once
#include "MShowCommon.h"
#include "MClock.h"
#include "MPreviewView.h"
#include "MPacketQueue.h"
#include "Control/TinyMenu.h"
using namespace TinyUI;

namespace MShow
{
	class MPreviewView;
	/// <summary>
	/// Ԥ��
	/// </summary>
	class MPreviewController : public TinyTaskBase
	{
		friend class MShowController;
		friend class MVideoController;
		friend class MImageController;
		friend class MRTMPEncoder;
		DISALLOW_COPY_AND_ASSIGN(MPreviewController)
	public:
		MPreviewController(MPreviewView& view, MClock& clock);
		virtual ~MPreviewController();
		BOOL		Initialize();
		BOOL		SetPulgSize(const TinySize& size);
		TinySize	GetPulgSize() const;
		void		SetVideoFPS(INT	videoFPS);
		INT			GetVideoFPS() const;
		void		Render();
		BYTE*		GetPointer();
		DWORD		GetSize();
		MPacketAllocQueue&	GetVideoQueue();
	public:
		void		Enter();
		void		Leave();
		BOOL		Add(DX11Element2D* ps);
		BOOL		Remove(DX11Element2D* ps);
		BOOL		Move(DX11Element2D* ps, BOOL bUp);
		BOOL		Bring(DX11Element2D* ps, BOOL bTop);
		BOOL		Find(DX11Element2D* ps);
		BOOL		Lock(DWORD dwMS);
	public:
		BOOL		Submit();
		BOOL		Close(DWORD dwMS) OVERRIDE;
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
		void	GetEvents(vector<HANDLE>&handles);
	private:
		DX11Element2D*	HitTest(const TinyPoint& pos);//ͼƬ����
	private:
		BOOL							m_bBreak;
		BOOL							m_bTracking;
		BOOL							m_bPopup;
		DWORD							m_dwSize;
		INT								m_videoFPS;
		TinySize						m_pulgSize;
		TinyMenu						m_popup;
		TinyEvent						m_event;
		DX11Element2D*					m_current;
		MClock&							m_clock;
		MPreviewView&					m_view;
		MPacketAllocQueue				m_videoQueue;
		DX11Graphics2D					m_graphics;
		DX11Image2D						m_handles[8];
		DX11RenderView					m_renderView;
		TinyPerformanceTimer			m_timeQPC;
		TinyArray<DX11Element2D*>		m_array;
		TinyArray<HANDLE>				m_events;
		TinyScopedArray<BYTE>			m_bits;
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



