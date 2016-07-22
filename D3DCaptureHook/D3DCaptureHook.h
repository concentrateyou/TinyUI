#pragma once
#include "D3DCommon.h"
#include "D3DCapture.h"
#include "SharedMemory.h"

namespace D3D
{

#define CAPTURE_MEMORY          TEXT("Local\\CaptureMemory")
#define TEXTURE_MUTEX1          TEXT("TextureMutex1")
#define TEXTURE_MUTEX2          TEXT("TextureMutex2")

	class CD3DCapture
	{
	public:
		CD3DCapture();
		~CD3DCapture();
	public:
		BOOL Attach(HMODULE hModule);
		BOOL Detach();
	private:
		static DWORD WINAPI CaptureLoop(LPVOID ps);
	private:
		HANDLE			m_hCapture;
		HANDLE			m_hMain;
		CSharedMemory	m_memory;
		CMutex			m_mutex1;
		CMutex			m_mutex2;
		D3D9Capture		m_d3d9Capture;
	};
}