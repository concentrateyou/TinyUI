#pragma once
#include "MFLVTask.h"
using namespace DXFramework;

namespace MShow
{
	/// <summary>
	/// DX��Ⱦ�ؼ�
	/// </summary>
	class DXView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(DXView)
	public:
		DXView();
		virtual ~DXView();
		//5����������
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		HICON RetrieveIcon() OVERRIDE;
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		//�¼�
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		DX2D		m_d2d;
		MFLVTask	m_task;
	};
}


