#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	class DX11CaptureDATA
	{
		DISALLOW_COPY_AND_ASSIGN(DX11CaptureDATA)
	public:
		DX11CaptureDATA();
		~DX11CaptureDATA();
		BOOL				Create(ID3D11Device *device, INT cx, INT cy, DXGI_FORMAT dxgiFormat);
		void				Destory();
		BOOL				Enter();
		void				Leave();
		BOOL				TestF(INT32 val) const;
		void				SetF(INT32 val);
		void				CrlF(INT32 val);
		UINT32				GetPitch() const;
		ID3D11Texture2D*	GetCopy2D();
		ID3D11Texture2D*	GetTexture2D();
	private:
		volatile INT32				m_iFlag;
		UINT32						m_linesize;
		TinySize					m_size;
		TinyLock					m_lock;
		TinyComPtr<ID3D11Texture2D>	m_copy2D;
		TinyComPtr<ID3D11Texture2D>	m_texture2D;
	};

	class DX11GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX11GraphicsCapture)
	public:
		DX11GraphicsCapture(DX& dx);
		~DX11GraphicsCapture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Draw(IDXGISwapChain *swap, UINT flags);
		BOOL DX11GPUHook(ID3D11Device *swap);
		BOOL DX11CPUHook(ID3D11Device *swap);
	private:
		DX11CaptureDATA* GetDX11CaptureDATA(LPVOID& data);
		void OnMessagePump();
		void QueryCopy(ID3D11DeviceContext *context);
	public:
		DX&								m_dx;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		volatile INT32					m_currentCopy;
		TLS								m_tls;
		BOOL							m_bGPU;
		DXGI_FORMAT						m_dxgiFormat;
		HANDLE							m_handle;
		HMODULE							m_hD3D11;
		TinyLock						m_lock;
		TinyEvent						m_copy;
		TinyEvent						m_stop;
		TinyWorker						m_captureTask;
		DX11CaptureDATA*				m_captures[NUM_BUFFERS];
		TinyComPtr<ID3D11Texture2D>		m_texture2D;
	};
	SELECTANY extern DX11GraphicsCapture g_dx11(g_dx);
}

