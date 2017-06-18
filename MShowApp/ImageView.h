#pragma once
#include "MPreviewController.h"
#include "MImageModel.h"
using namespace DXFramework;

namespace MShow
{
	class MPreviewController;

	class ImageView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(ImageView)
	public:
		ImageView(MPreviewController& controller);
		virtual ~ImageView();
		//5����������
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		HICON	RetrieveIcon() OVERRIDE;
		BOOL	Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		DX2D&	GetD2D();
		//�¼�
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnRButtonDBClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void OnVideo(BYTE* bits, LONG size);
		void DrawView();
	private:
		DX2D						m_dx2d;
		MPreviewController&			m_controller;
		TinyComPtr<ID2D1Bitmap1>	m_bitmap1;
		TinyScopedPtr<MImageModel>	m_model;
		
	};
}


