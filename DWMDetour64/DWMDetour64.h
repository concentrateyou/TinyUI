#pragma once
#include "Common/TinyCommon.h"
#include "IO/TinySharedMemory.h"
#include "IO/TinyTaskBase.h"
using namespace TinyUI;

namespace DWMDetour64
{
#define DWM_WINDOWCLASS			TEXT("DWMCapture")

	class DWMCapture
	{
	public:
		DWMCapture();
		~DWMCapture();
		BOOL Attach(HMODULE hModule);
		BOOL Detach(HMODULE hModule);
	private:
		void BeginCapture();
		void EndCapture();
		void OnMessagePump();
	private:
		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		IO::TinyTaskBase	m_task;
		HINSTANCE			m_hInstance;
		HWND				m_hWNDD3D;
	};
}


