#pragma once
#include "MShowCommon.h"
#include "MFLVScene.h"
#include "DXView.h"
#include "MGIFScene.h"
using namespace TinyUI;

namespace MShow
{
	class DXView;

	class MShowController
	{
		friend class MGIFScene;
		friend class MFLVScene;
		DISALLOW_COPY_AND_ASSIGN(MShowController)
	public:
		MShowController(DXView& view);
		virtual ~MShowController();
		BOOL Initialize(LPCSTR pzURL);
		void Uninitialize();
		void Draw(MElement* ps);
		TinyTimerQueue* GetTimerQueue();
	private:
		void	OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	private:
		DX2D		m_d2d;
		DXView&		m_view;
		TinyArray<MElement*>	m_scenes;
		TinyScopedPtr<Delegate<void(ID2D1Bitmap1*, INT, MElement*)>> m_onVideo;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSize;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonUp;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onRButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseMove;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseLeave;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSetCursor;
	private:
		TinyTimerQueue	m_queue;
	};
}



