#pragma once
#include "DXFramework.h"
#include "DX11CaptureSource.h"

namespace DXFramework
{
	class DX11CaptureTask
	{
	public:
		DX11CaptureTask();
		~DX11CaptureTask();
		BOOL Initialize(DX11* dx11);
		DX11* GetDX11() const;
		DX11CaptureSource*	GetSource();
		static DWORD Loop(LPVOID lpUnused);
	private:
		HANDLE	m_hTask;
		DX11CaptureSource m_capture;
		DX11*	m_dx11;
	};
}
