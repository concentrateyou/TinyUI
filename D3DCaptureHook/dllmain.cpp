// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "D3DCaptureHook.h"

D3D::CD3DCapture g_capture;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_capture.Attach(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		g_capture.Detach();
		break;
	}
	return TRUE;
}

