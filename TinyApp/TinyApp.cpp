// TinyApp.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TinyApp.h"
#include "Control/TinyFrameUI.h"
#include "Windowless/TinyVisualHWND.h"
#include "Database/TinyAdo.h"
#include "Network/TinyConnector.h"
#include <algorithm>

#pragma comment(lib,"TinyUI.lib")
using namespace TinyUI;

class ListV
{
public:
	ListV()
		:V(0)
	{
		TRACE("ListV默认构造函数\n");
	}
	~ListV()
	{
		TRACE("ListV析构函数:%d\n",V);
	}
	explicit ListV(INT v)
		:V(v)
	{
		TRACE("ListV构造函数:%d\n",V);
	}
	ListV(const ListV& other)
		:V(other.V)
	{
		TRACE("ListV拷贝构造函数:%d\n",V);
	}
	ListV& operator=(const ListV& other)
	{
		TRACE("ListV赋值构造函数\n");
		return *this;
	}
	ListV(ListV&& other)
		:V(other.V)
	{
		TRACE("ListV移动构造函数\n");
	}

	static ListV GetV(INT v)
	{
		return ListV(v);
	}

	static ListV GetV1(INT v)
	{
		ListV v1(v);
		return v1;
	}

public:
	INT V;
};
template<>
class DefaultTraits < ListV >
{
public:
	static INT  Compare(const ListV& element1, const ListV& element2)
	{
		if (element1.V == element2.V)
		{
			return 0;
		}
		else if (element1.V > element2.V)
		{
			return 1;
		}
		return -1;
	}
};


INT APIENTRY _tWinMain(HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	INT       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WSADATA   wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);

	HRESULT hRes = OleInitialize(NULL);
	
	TinyLinkList<ListV> linkList;
	for (int i = 0; i < 100;i++)
	{
 		ListV lv(i);
		if (i == 99)
		{
			linkList.InsertFirst(lv);
		}
		else
		{
			linkList.InsertFirst(lv);
		}
	}
	//CList
	::DefWindowProc(NULL, 0, 0, 0L);

	TinyApplication::GetInstance()->Initialize(hInstance, lpCmdLine, nCmdShow, MAKEINTRESOURCE(IDC_TINYAPP));

	TinyMessageLoop theLoop;
	TinyApplication::GetInstance()->AddMessageLoop(&theLoop);
	TinyVisualHWND uiImpl;
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