#pragma once
#include "Control/TinyCustomDialog.h"
#include "Control/TinyTextBox.h"
#include "MShowCommon.h"
using namespace DXFramework;

namespace MShow
{
	/// <summary>
	/// ��ƵԴView
	/// </summary>
	class MAudioView : public TinyControl
	{
		DISALLOW_COPY_AND_ASSIGN(MAudioView)
	public:
		MAudioView();
		virtual ~MAudioView();
		//5����������
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		HICON	RetrieveIcon() OVERRIDE;
		BOOL	Create(HWND hParent, INT x, INT y, INT cx, INT cy);
		//�¼�
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnErasebkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	};
}


