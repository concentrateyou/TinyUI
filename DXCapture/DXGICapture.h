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
		static DXGICapture& Instance();
	};
}

