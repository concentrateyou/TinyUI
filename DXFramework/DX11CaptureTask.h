#pragma once
#include "DXFramework.h"
#include "DX11CaptureSource.h"
#include "DX11Image.h"

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
		DX11Image*			GetTexture();
		static DWORD Loop(LPVOID lpUnused);
	private:
		DX11*				m_dx11;
		HANDLE				m_hTask;
		DX11CaptureSource	m_source;
	};
}
