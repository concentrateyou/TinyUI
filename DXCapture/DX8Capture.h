#pragma once
#include "d3d8.h"
#include "DX.h"

namespace DXCapture
{
#define NUM_BUFFERS 3
	class DX8CaptureDATA
	{
	public:
		DX8CaptureDATA();
		~DX8CaptureDATA();
		BOOL Create(IDirect3DDevice8* pThis, INT cx, INT cy, D3DFORMAT format);
		void Destory();
		BOOL Update();
		BOOL Enter();
		void Leave();
		void SetCopying(BOOL bCopy);
		BOOL IsCopying() const;
		BYTE* GetPointer() const;
		DWORD GetPitch() const;
		IDirect3DSurface8*	GetSurface();
	private:
		IDirect3DSurface8*	m_surface;
		BOOL				m_copying;
		TinyLock			m_lock;
		DWORD				m_dwPitch;
		BYTE*				m_bits;
		TinySize			m_size;
	};
	/// <summary>
	/// XX8纹理数据捕获
	/// </summary>
	class DX8Capture
	{
	public:
		DX8Capture(DX& dx);
		~DX8Capture();
		BOOL Initialize(HWND hWND);
		BOOL Render(IDirect3DDevice8 *pThis);
		void Reset();
		BOOL Setup(IDirect3DDevice8 *pThis);
		BOOL DX8CPUHook(IDirect3DDevice8 *pThis);
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
		DX8CaptureDATA					m_captures[NUM_BUFFERS];
		UINT							m_currentCapture;
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

