#pragma once
#include "DXFramework.h"
#include "DX9.h"
#include "DX9Image2D.h"

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
		DX9&	GetDX9();
		void	Present();
		BOOL	Initialize(HWND hWND, const TinySize& size);
		BOOL	DrawImage(DX9Image2D* ps, FLOAT ratioX = 1.0F, FLOAT ratioY = 1.0F);
	private:
		DX9		m_dx9;
	};
}

