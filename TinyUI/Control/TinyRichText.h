#pragma once
#include "../Common/TinyObject.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	class TinyRichText : public TinyControl
	{
		DECLARE_DYNAMIC(TinyRichText);
	public:
		TinyRichText();
		virtual ~TinyRichText();
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
		DWORD RetrieveStyle() OVERRIDE;
		DWORD RetrieveExStyle() OVERRIDE;
		BOOL Create(HWND hParent, INT x, INT y, INT cx, INT cy);
	public:
		BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) OVERRIDE;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		TinyScopedPtr<TinyTextHost> m_texthost;
	};
}