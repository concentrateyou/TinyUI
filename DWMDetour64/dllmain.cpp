// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "DWMDetour.h"
#include "DWMCapture.h"

DWM::DWMCapture g_capture;

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		g_capture.Attach(hModule);
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		g_capture.Detach(hModule);
	}
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

