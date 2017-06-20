#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "Control/TinyMenu.h"
#include "MElement.h"
using namespace TinyUI;

namespace MShow
{
	class MPreviewView;
	/// <summary>
	/// ‘§¿¿ΩÁ√Ê
	/// </summary>
	class MPreviewController
	{
		friend class MGIFScene;
		friend class MFLVModel;
		DISALLOW_COPY_AND_ASSIGN(MPreviewController)
	public:
		MPreviewController(MPreviewView& view);
		virtual ~MPreviewController();
		BOOL	Initialize(const TinySize& pulgSize);
		void	Uninitialize();
		BOOL	Add(MElement* element);
		BOOL	Remove(MElement* element);
		void	BringToTop(MElement* element);
		void	BringToBottom(MElement* element);
		void	MoveUp(MElement* element);
		void	MoveDown(MElement* element);
		void	Draw(MElement* ps);
	public:
		DX2D&	GetD2D();
		MPreviewView& GetView();
	private:
		void	OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void	OnMenuClick(void*, INT wID);
	private:
		MElement* HitTest(const TinyPoint& pos);
	private:
		DX2D						m_dx2d;
		TinySize					m_pulgSize;
		TinySize					m_size;
		TinyMenu					m_menu;
		MPreviewView&				m_view;
		TinyArray<MElement*>		m_models;
		TinyComPtr<ID2D1Bitmap>		m_box;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap;
		TinyComPtr<ID2D1Bitmap1>	m_bitmapCopy;
		TinyScopedPtr<Delegate<void(void*, INT)>>				   m_onMenuClick;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSize;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onLButtonUp;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onRButtonDown;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseMove;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onMouseLeave;
		TinyScopedPtr<Delegate<void(UINT, WPARAM, LPARAM, BOOL&)>> m_onSetCursor;
	private:
		TinyLock		m_lock;
		MElement*		m_lastElement;
	};
}



