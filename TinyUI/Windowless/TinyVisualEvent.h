#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"

namespace TinyUI
{
	namespace Windowless
	{
		enum MouseButtons
		{
			NONE,
			LBUTTON,
			RBUTTON,
			MBUTTON
		};

		class EventArgs
		{
		public:
			EventArgs() = DEFAULT;
		};

		class MouseEventArgs : public EventArgs
		{
		public:
			MouseEventArgs(MouseButtons button, INT clicks, INT x, INT y, INT delta);
		public:
			MouseButtons	Button();
			INT				X();
			INT				Y();
			INT				Delta();
			INT				Clicks();
		private:
			MouseButtons	m_button;
			INT				m_clicks;
			INT				m_x;
			INT				m_y;
			INT				m_delta;
		};

		class KeyEventArgs : public EventArgs
		{
		public:
			KeyEventArgs(DWORD dwKey);
			DWORD	Key();
		private:
			DWORD	m_dwKey;
		};
	}
}



