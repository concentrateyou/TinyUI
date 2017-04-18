#include "stdafx.h"
#include "DXInput.h"

namespace DXFramework
{
	DXInput::DXInput()
	{
	}

	DXInput::~DXInput()
	{
	}
	BOOL DXInput::Initialize(HINSTANCE hInstance, HWND hWND, const TinySize& size)
	{
		HRESULT hRes = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_input, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_input->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_keyboard->SetCooperativeLevel(hWND, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_keyboard->Acquire();
		if (hRes != S_OK)
			return FALSE;
		hRes = m_input->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_mouse->SetDataFormat(&c_dfDIMouse);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_mouse->SetCooperativeLevel(hWND, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (hRes != S_OK)
			return FALSE;
		hRes = m_mouse->Acquire();
		if (hRes != S_OK)
			return FALSE;
		m_size = size;
		return TRUE;
	}
}