#include "stdafx.h"
#include "DXGICapture.h"
#include "DX10Capture.h";
#include "DX101Capture.h";
#include "DX11Capture.h";

namespace DXCapture
{
	DXGICapture::DXGICapture()
	{

	}
	DXGICapture::~DXGICapture()
	{

	}
	BOOL DXGICapture::Initialize(HWND hWND)
	{
		if (DX10Capture::Instance().Initialize(hWND))
			return TRUE;
		if (DX101Capture::Instance().Initialize(hWND))
			return TRUE;
		if (DX11Capture::Instance().Initialize(hWND))
			return TRUE;
		return FALSE;
	}
	DXGICapture& DXGICapture::Instance()
	{
		static DXGICapture instance;
		return instance;
	}
}