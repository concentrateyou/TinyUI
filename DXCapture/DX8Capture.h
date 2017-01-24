#pragma once
#include "d3d8.h"
#include "DX.h"

namespace DXCapture
{
	typedef IDirect3D8* (WINAPI*D3D8CREATEPROC)(UINT);
	HRESULT STDMETHODCALLTYPE DX8EndScene(IDirect3DDevice8 *device);
	HRESULT STDMETHODCALLTYPE DX8Present(IDirect3DDevice8* device, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
	HRESULT STDMETHODCALLTYPE DX8Reset(IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	BOOL ConvertPixelFormat(LPBYTE src, DWORD srcPitch, D3DFORMAT srcFormat, DWORD dstCX, DWORD dstCY, LPBYTE dst);
#define NUMBER_OF_BUFFER 3

	class DX8Surface
	{
	public:
		DX8Surface();
		~DX8Surface();
	public:
		BOOL	Create(IDirect3DDevice8 *d3d, SharedCaptureDATA* pDATA);
		void	Destory();
		BOOL	LockRect();
		BOOL	UnlockRect();
		BYTE*	GetPointer();
		INT		GetPitch();
		TinyLock&	GetLock();
		IDirect3DSurface8*	GetSurface();
	private:
		D3DLOCKED_RECT					m_lockedRect;
		TinyLock						m_lock;
		TinyComPtr<IDirect3DSurface8>	m_surface;
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
		BOOL Render(IDirect3DDevice8 *device);
		void Reset();
		BOOL Setup(IDirect3DDevice8 *pThis);
		BOOL DX8CPUHook(IDirect3DDevice8 *device);
	private:
		void OnMessagePump();
	public:
		DWORD							m_dwCurrentCapture;
		DWORD							m_dwCurrent;
		IO::TinyTaskBase				m_captureTask;
		LPBYTE							m_textures[2];
		DX8Surface						m_surfaces[NUMBER_OF_BUFFER];
		LPVOID							m_currentPointer;
		SharedCaptureDATA				m_captureDATA;
		BOOL							m_bCapturing;
		BOOL							m_bTextures;
		TinyEvent						m_copy;
		TinyEvent						m_close;
		TinyDetour						m_dX8EndScene;
		TinyDetour						m_dX8Present;
		TinyDetour						m_dX8Reset;
		HMODULE							m_hD3D8;
		DX&								m_dx;
	};
	SELECTANY extern DX8Capture g_dx8(g_dx);
}

