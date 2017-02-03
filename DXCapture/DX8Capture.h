#pragma once
#include "d3d8.h"
#include "DX.h"

namespace DXCapture
{
#define NUM_BUFFERS 3
	/// <summary>
	/// XX8�������ݲ���
	/// </summary>
	class DX8Capture
	{
	public:
		DX8Capture(DX& dx);
		~DX8Capture();
		BOOL Initialize(HWND hWND);
		BOOL Render(IDirect3DDevice8 *device);
		void Reset();
		BOOL Setup(IDirect3DDevice8 *pThis);
		BOOL DX8CPUHook(IDirect3DDevice8 *device);
	private:
		void OnMessagePump();
	public:
		LPVOID							m_currentPointer;
		SharedCaptureDATA				m_captureDATA;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		D3DFORMAT						m_d3dFormat;
		DXGI_FORMAT						m_dxgiFormat;
		IO::TinyTaskBase				m_captureTask;

		LPBYTE							m_textures[2];
		IDirect3DSurface8*				m_surfaces[NUM_BUFFERS];
		BOOL							m_copyings[NUM_BUFFERS];
		TinyLock						m_mutexs[NUM_BUFFERS];
		TinyLock						m_lock;
		void *volatile					m_currentBits;
		DWORD							m_dwPitch;
		DWORD							m_dwCurrentTexture;
		DWORD							m_dwCurrentCapture;
		TinyEvent						m_copy;
		TinyEvent						m_close;
		TinyDetour						m_dX8EndScene;
		TinyDetour						m_dX8Present;
		TinyDetour						m_dX8Reset;
		TinyDetour						m_dX8Release;
		HMODULE							m_hD3D8;
		DX&								m_dx;
	};
	SELECTANY extern DX8Capture g_dx8(g_dx);
}

