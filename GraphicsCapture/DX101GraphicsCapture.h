#pragma once
#include "DX.h"

namespace GraphicsCapture
{
	class DX101CaptureDATA
	{
		DISALLOW_COPY_AND_ASSIGN(DX101CaptureDATA)
	public:
		DX101CaptureDATA();
		~DX101CaptureDATA();
		BOOL				Create(ID3D10Device *device, INT cx, INT cy, DXGI_FORMAT dxgiFormat);
		void				Destory();
		BOOL				Enter();
		void				Leave();
		BOOL				TestF(INT32 val) const;
		void				SetF(INT32 val);
		void				CrlF(INT32 val);
		UINT32				GetPitch() const;
		ID3D10Texture2D*	GetCopy2D();
		ID3D10Texture2D*	GetTexture2D();
	private:
		volatile INT32				m_iFlag;
		UINT32						m_linesize;
		TinySize					m_size;
		TinyLock					m_lock;
		TinyComPtr<ID3D10Texture2D>	m_copy2D;
		TinyComPtr<ID3D10Texture2D>	m_texture2D;
	};

	class DX101GraphicsCapture
	{
		DISALLOW_COPY_AND_ASSIGN(DX101GraphicsCapture)
	public:
		DX101GraphicsCapture(DX& dx);
		~DX101GraphicsCapture();
		BOOL Initialize(HWND hWND, TinyComPtr<IDXGISwapChain>& swap);
		void Reset();
		BOOL Setup(IDXGISwapChain *swap);
		BOOL Draw(IDXGISwapChain *swap, UINT flags);
		BOOL DX101GPUHook(ID3D10Device1 *device);
		BOOL DX101CPUHook(ID3D10Device1 *device);
	private:
		DX101CaptureDATA* GetDX101CaptureDATA(LPVOID& data);
		void OnMessagePump();
		void QueryCopy(ID3D10Device *device);
	public:
		DX&								m_dx;
		volatile BOOL					m_bCapturing;
		volatile BOOL					m_bActive;
		volatile INT32					m_currentCopy;
		BOOL							m_bGPU;
		DXGI_FORMAT						m_dxgiFormat;
		TLS								m_tls;
		HANDLE							m_handle;
		TinyLock						m_lock;
		TinyEvent						m_copy;
		TinyEvent						m_stop;
		TinyWorker						m_captureTask;
		DX101CaptureDATA*				m_captures[NUM_BUFFERS];
		TinyComPtr<ID3D10Texture2D>		m_texture2D;
	};
	SELECTANY extern DX101GraphicsCapture g_dx101(g_dx);
}

