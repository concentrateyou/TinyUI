// D3DCaptureHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "D3DCaptureHook.h"

namespace D3D
{
	CD3DCapture::CD3DCapture()
		:m_hMain(NULL),
		m_hCapture(NULL)
	{

	}
	CD3DCapture::~CD3DCapture()
	{

	}
	BOOL CD3DCapture::Attach(HMODULE hModule)
	{
		CHAR name[MAX_PATH];
		HANDLE hThread = NULL;
		GetModuleFileName(hModule, name, MAX_PATH);
		LoadLibrary(name);
		m_hMain = OpenThread(THREAD_ALL_ACCESS, NULL, GetCurrentThreadId());
		if (!(m_hCapture = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CD3DCapture::CaptureLoop, (LPVOID)this, 0, 0)))
		{
			CloseHandle(m_hMain);
			return FALSE;
		}
		return TRUE;
	}
	BOOL CD3DCapture::Detach()
	{
		if (m_hCapture)
		{
			WaitForSingleObject(m_hCapture, 300);
			CloseHandle(m_hCapture);
		}

	}
	DWORD WINAPI CD3DCapture::CaptureLoop(LPVOID ps)
	{
		CD3DCapture* _this = reinterpret_cast<CD3DCapture*>(ps);
		//等待主线程初始化完成
		if (_this->m_hMain)
		{
			WaitForSingleObject(_this->m_hMain, 150);
			CloseHandle(_this->m_hMain);
		}
		_this->m_memory.Create(CAPTURE_MEMORY, sizeof(CaptureEntry));
		if (!_this->m_memory.Map())
			return FALSE;
		_this->m_mutex1.Open(MUTEX_ALL_ACCESS, FALSE, TEXTURE_MUTEX1);
		_this->m_mutex2.Open(MUTEX_ALL_ACCESS, FALSE, TEXTURE_MUTEX2);
		_this->m_d3d9Capture.Initialize(NULL);
		return TRUE;
	}
}