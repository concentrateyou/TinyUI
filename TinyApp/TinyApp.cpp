// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "Common/TinyModule.h"
#include "SkinFramework/TinySkinWindow.h"
#include "Windowless/TinyVisualHWND.h"
#include "Database/TinyAdo.h"
#include "Network/TinyConnector.h"
#include "Common/TinyLogging.h"
#include "Media/TinyVideoCapture.h"
#include "Common/TinyHook.h"
#include "Common/TinyTemplate.h"
#include "Common/TinyEvent.h"
#include <algorithm>
#include <map>
#include <functional>

#pragma comment(lib,"TinyUI.lib")
using namespace TinyUI;

//IBaseFilter* GetFilter()
//{
//	IBaseFilter* bfs = NULL;
//	TinyArray<TinyString> devices;
//	Media::TinyVideoCapture::GetDeviceNames(devices);
//	if (devices.GetSize() > 0)
//	{
//		Media::TinyVideoCapture::GetDeviceFilter(devices[0].STR(), &bfs);
//	}
//	return bfs;
//}

class TestEvent
{

};

void Show1()
{
	TRACE("Call Show1");
}

void Show2()
{
	TRACE("Call Show2");
}

INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	Delegate<void(void)>* ds = new Delegate<void(void)>(&Show1);

	SAFE_DELETE(ds);


	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	HRESULT hRes = OleInitialize(NULL);

	::DefWindowProc(NULL, 0, 0, 0L);
	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));
	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	TinyUI::Windowless::TinyVisualHWND uiImpl;
	uiImpl.Create(NULL, 50, 50, 400, 500);
	uiImpl.ShowWindow(nCmdShow);
	uiImpl.UpdateWindow();
	INT loopRes = theLoop.MessageLoop();
	TinyApplication::GetInstance()->RemoveMessageLoop();
	TinyApplication::GetInstance()->Uninitialize();
	OleUninitialize();
	WSACleanup();
	return loopRes;
};