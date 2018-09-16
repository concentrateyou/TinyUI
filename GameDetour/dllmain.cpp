// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "GameDetour.h"

GameDetour::GameDetour g_capture;

typedef BOOL(WINAPI *UHWHEXPROC)(HHOOK);
extern "C" __declspec(dllexport) LRESULT CALLBACK GameDetourProc(int code, WPARAM wParam, LPARAM lParam)
{
	static BOOL hooking = TRUE;
	MSG *msg = (MSG*)lParam;
	if (hooking && msg->message == (WM_USER + 432))
	{
		HMODULE hUSER32 = GetModuleHandle("USER32");
		UHWHEXPROC UnhookWindowsHookEx = (UHWHEXPROC)GetProcAddress(hUSER32, TEXT("UnhookWindowsHookEx"));
		if (UnhookWindowsHookEx != NULL)
		{
			UnhookWindowsHookEx((HHOOK)msg->lParam);
		}
		hooking = FALSE;
	}
	return CallNextHookEx(0, code, wParam, lParam);
}

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
	case DLL_PROCESS_DETACH:
		g_capture.Detach(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

