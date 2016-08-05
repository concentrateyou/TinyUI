#include "../stdafx.h"
#include "TinyVisualEvent.h"

namespace TinyUI
{
	namespace Windowless
	{
		MouseEventArgs::MouseEventArgs(MouseButtons button, INT clicks, INT x, INT y, INT delta)
			:m_button(button),
			m_clicks(clicks),
			m_x(x),
			m_y(y),
			m_delta(delta)
		{
		}
		MouseButtons MouseEventArgs::Button()
		{
			return m_button;
		}
		INT	MouseEventArgs::X()
		{
			return m_x;
		}
		INT	MouseEventArgs::Y()
		{
			return m_y;
		}
		INT	MouseEventArgs::Delta()
		{
			return m_delta;
		}
		INT	MouseEventArgs::Clicks()
		{
			return m_clicks;
		}
		//////////////////////////////////////////////////////////////////////////
		KeyEventArgs::KeyEventArgs(DWORD dwKey, DWORD dwRepCnt, DWORD dwFlags)
			:m_dwKey(dwKey),
			m_dwRepCnt(dwRepCnt),
			m_dwFlags(dwFlags)
		{

		}
		DWORD KeyEventArgs::Key()
		{
			return m_dwKey;
		}
		DWORD KeyEventArgs::RepCnt()
		{
			return m_dwRepCnt;
		}
		DWORD KeyEventArgs::Flag()
		{
			return m_dwFlags;
		}
		//////////////////////////////////////////////////////////////////////////
		FocusEventArgs::FocusEventArgs(BOOL bFocus)
			:m_bFocus(bFocus)
		{

		}
		BOOL FocusEventArgs::IsFocus() const
		{
			return m_bFocus;
		}
		//////////////////////////////////////////////////////////////////////////
		CaptureEventArgs::CaptureEventArgs(BOOL bCapture)
			:m_bCapture(bCapture)
		{

		}
		BOOL CaptureEventArgs::IsCapture() const
		{
			return m_bCapture;
		}
		//////////////////////////////////////////////////////////////////////////
		ActiveEventArgs::ActiveEventArgs(BOOL bActive)
			:m_bActive(bActive)
		{

		}
		BOOL ActiveEventArgs::IsActive() const
		{
			return m_bActive;
		}
	}
}
