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
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
		LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) OVERRIDE;
	private:
		TinyTextHost	m_texthost;
		TinySize		m_size;
	};
}