#pragma once
#include "../Control/TinyControl.h"
#include "../Render/TinyImage.h"

namespace TinyFramework
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
			EventArgs() = default;
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
			KeyEventArgs(DWORD dwKey, DWORD	m_dwRepCnt, DWORD m_dwFlags);
			DWORD	Key();
			DWORD	RepCnt();
			DWORD	Flag();
		private:
			DWORD	m_dwKey;
			DWORD	m_dwRepCnt;
			DWORD	m_dwFlags;
		};
		class FocusEventArgs :public EventArgs
		{
		public:
			FocusEventArgs(BOOL bFocus);
			BOOL IsFocus() const;
		private:
			BOOL m_bFocus;
		};
		class CaptureEventArgs :public EventArgs
		{
		public:
			CaptureEventArgs(BOOL bCapture);
			BOOL IsCapture() const;
		private:
			BOOL m_bCapture;
		};
		class ActiveEventArgs :public EventArgs
		{
		public:
			ActiveEventArgs(BOOL bActive);
			BOOL IsActive() const;
		private:
			BOOL m_bActive;
		};
	}
}



