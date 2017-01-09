#pragma once
#include "DXFramework.h"
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

namespace DXFramework
{
	class DXInput
	{
	public:
		DXInput();
		virtual ~DXInput();
		BOOL	Initialize(HINSTANCE hInstance, HWND hWND, const TinySize& size);
	protected:
		TinyPoint						m_position;
		TinySize						m_size;
		TinyComPtr<IDirectInput8>		m_input;
		TinyComPtr<IDirectInputDevice8> m_keyboard;
		TinyComPtr<IDirectInputDevice8> m_mouse;
		DIMOUSESTATE					m_mouseState;
		BYTE							m_keyboardState[256];
	};
}
