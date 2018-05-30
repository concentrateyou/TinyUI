#pragma once
#include "TinyControl.h"

namespace TinyFramework
{
	/// <summary>
	/// ÒõÓ°´°¿Ú
	/// </summary>
	class TinyShadow : public TinyControl
	{
		DECLARE_DYNAMIC(TinyShadow)
	public:
		TinyShadow();
		virtual ~TinyShadow();
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		BOOL Create(HWND hParent, HWND hOwner);
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	public:
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		void Draw(HDC hDC);
	};
}