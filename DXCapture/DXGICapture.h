#pragma once
#include "DXCommon.h"

namespace DXCapture
{
	class DX10Capture;
	class DX101Capture;
	class DX11Capture;

	class DXGICapture
	{
	public:
		DXGICapture();
		~DXGICapture();
		BOOL Initialize(HWND hWND);
		void Reset(BOOL bRelease = TRUE);
		static DXGICapture& Instance();
	private:
		BOOL m_bDX10;
		BOOL m_bDX101;
		BOOL m_bDX11;
	};
}

