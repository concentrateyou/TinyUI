#pragma once
#include "MPreviewController.h"
#include "MImageModel.h"
using namespace DXFramework;

namespace MShow
{
	class MPreviewController;

	class MImageView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(MImageView)
	public:
		MImageView(MPreviewController& controller,DWORD dwIndex);
		virtual ~MImageView();
		//5个创建函数
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		HICON	RetrieveIcon() OVERRIDE;
		BOOL	Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		DX2D&	GetD2D();
		//事件
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void	OnVideo(BYTE* bits, LONG size);
		void	OnAdd();
		void	OnRemove();
		void	OnMenuClick(void*, INT wID);
		void	DrawView();
	private:
		DWORD						m_dwIndex;
		DX2D						m_dx2d;
		TinyMenu					m_menu;
		MPreviewController&			m_controller;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap1;
		TinyScopedPtr<MImageModel>	m_model;
		TinyScopedPtr<Delegate<void(void*, INT)>>	m_onMenuClick;
	};
}


