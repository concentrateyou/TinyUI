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
		KeyEventArgs::KeyEventArgs(DWORD dwKey)
			:m_dwKey(dwKey)
		{

		}
		DWORD	KeyEventArgs::Key()
		{
			return m_dwKey;
		}
	}
}
