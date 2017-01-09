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
		if (FAILED(hRes))
			return FALSE;
		hRes = m_input->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_keyboard->SetCooperativeLevel(hWND, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_keyboard->Acquire();
		if (FAILED(hRes))
			return FALSE;
		hRes = m_input->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_mouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_mouse->SetCooperativeLevel(hWND, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if (FAILED(hRes))
			return FALSE;
		hRes = m_mouse->Acquire();
		if (FAILED(hRes))
			return FALSE;
		m_size = size;
		return TRUE;
	}
}