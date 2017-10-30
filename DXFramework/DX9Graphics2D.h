#pragma once
#include "DXFramework.h"
#include "DX9.h"
#include "DXCamera.h"

namespace DXFramework
{
	/// <summary>
	/// DX9»­²¼
	/// </summary>
	class DX9Graphics2D
	{
	public:
		DX9Graphics2D();
		virtual ~DX9Graphics2D();
		BOOL	Initialize(HWND hWND, const TinySize& size);
	private:
		DX9					m_dx9;
		DXCamera			m_camera;
	};
}

