// DXApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DXApp.h"
#include "DXApplication.h"
using namespace DXFramework;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);



	DXApplication app;
	app.Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_DXAPP));
	return app.Run();
}

