#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	class DX10CaptureDATA
	{
		DISALLOW_COPY_AND_ASSIGN(DX10CaptureDATA)
	public:
		DX10CaptureDATA();
		~DX10CaptureDATA();
		BOOL				Create(ID3D10Device *device, INT cx, INT cy, DXGI_FORMAT dxgiFormat);
		void				Destory();
		BOOL				Enter();
		void				Leave();
		void				SetCopying(BOOL bCopy);
		BOOL				IsCopying() const;
		void				SetIssue(BOOL bIssue);
		BOOL				IsIssue() const;
		UINT32				GetPitch() const;
		ID3D10Texture2D*	GetCopy2D();
		ID3D10Texture2D*	GetTexture2D();
	private:
		volatile BOOL				m_bCopying;
		volatile BOOL				m_bIssue;
		UINT32						m_pitch;
		TinySize					m_size;
		TinyLock					m_lock;
		TinyComPtr<ID3D10Texture2D>	m_copy2D;
		TinyComPtr<ID3D10Texture2D>	m_texture2D;
	};

	class DX10GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX10GraphicsCapture)
	public:
		DX10GraphicsCapture(DX& dx);
		~DX10GraphicsCapture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Draw(IDXGISwapChain *swap, UINT flags);
		BOOL DX10GPUHook(ID3D10Device *device);
		BOOL DX10CPUHook(ID3D10Device *device);
	private:
		void OnMessagePump();
		void QueryCopy(ID3D10Device *device);
	public:
		DX&								m_dx;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		volatile INT32					m_current;
		volatile INT32					m_currentCopy;
		volatile LPVOID					m_bits;
		DXGI_FORMAT						m_dxgiFormat;
		HANDLE							m_handle;
		HMODULE							m_hD3D10;
		DWORD							m_dwCopy;
		LPBYTE							m_textures[2];
		TinyEvent						m_copy;
		TinyEvent						m_close;
		TinyWorker						m_captureTask;
		DX10CaptureDATA*				m_captures[NUM_BUFFERS];
		TinyComPtr<ID3D10Texture2D>		m_texture2D;
	};
	SELECTANY extern DX10GraphicsCapture g_dx10(g_dx);
}

