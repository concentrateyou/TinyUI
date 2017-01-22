#include "stdafx.h"
#include "DXGICapture.h"

namespace DXCapture
{
	HRESULT STDMETHODCALLTYPE DX_DXGISwapPresent(IDXGISwapChain *pThis, UINT syncInterval, UINT flags)
	{
		g_dxgi.m_dxPresent.EndDetour();
		TinyComPtr<IUnknown> unknow;
		if (SUCCEEDED(pThis->GetDevice(__uuidof(IUnknown), (void**)&unknow)))
		{
			do
			{
				TinyComPtr<IUnknown> device;
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device), (void**)&device)))
				{
					g_dxgi.m_bDX10 = TRUE;
					if (g_dxgi.m_currentSwap == NULL)
					{
						g_dxgi.m_currentSwap = g_dx10.Setup(pThis) ? pThis : NULL;
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dxgi.m_currentSwap == pThis)
					{
						g_dx10.Render(pThis, flags);
					}
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device1), (void**)&device)))
				{
					g_dxgi.m_bDX101 = TRUE;
					if (g_dxgi.m_currentSwap == NULL)
					{
						g_dxgi.m_currentSwap = g_dx101.Setup(pThis) ? pThis : NULL;
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dxgi.m_currentSwap == pThis)
					{
						g_dx101.Render(pThis, flags);
					}
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D11Device), (void**)&device)))
				{
					g_dxgi.m_bDX11 = TRUE;
					if (g_dxgi.m_currentSwap == NULL)
					{
						g_dxgi.m_currentSwap = g_dx11.Setup(pThis) ? pThis : NULL;
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dxgi.m_currentSwap == pThis)
					{
						g_dx11.Render(pThis, flags);
					}
					break;
				}
			} while (0);
		}
		HRESULT hRes = pThis->Present(syncInterval, flags);
		g_dxgi.m_dxPresent.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX_DXGISwapResizeBuffers(IDXGISwapChain *pThis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
	{
		g_dxgi.m_dxResizeBuffers.EndDetour();
		TinyComPtr<IUnknown> unknow;
		if (SUCCEEDED(pThis->GetDevice(__uuidof(IUnknown), (void**)&unknow)))
		{
			do
			{
				TinyComPtr<IUnknown> device;
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device), (void**)&device)))
				{
					g_dxgi.m_bDX10 = TRUE;
					g_dx10.Release();
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device1), (void**)&device)))
				{
					g_dxgi.m_bDX101 = TRUE;
					g_dx101.Release();
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D11Device), (void**)&device)))
				{
					g_dxgi.m_bDX11 = TRUE;
					g_dx11.Release();
					break;
				}
			} while (0);
		}
		g_dxgi.m_currentSwap = NULL;
		HRESULT hRes = pThis->ResizeBuffers(bufferCount, width, height, giFormat, flags);
		g_dxgi.m_dxResizeBuffers.BeginDetour();
		return hRes;
	}

	DXGICapture::DXGICapture(DX10Capture& dx10, DX101Capture& dx101, DX11Capture& dx11)
		:m_bDX10(FALSE),
		m_bDX101(FALSE),
		m_bDX11(FALSE),
		m_dx10(dx10),
		m_dx101(dx101),
		m_dx11(dx11),
		m_currentSwap(NULL)
	{

	}
	DXGICapture::~DXGICapture()
	{

	}
	BOOL DXGICapture::Initialize(HWND hWND)
	{
		TinyComPtr<IDXGISwapChain> swap;
		if (m_dx10.Initialize(hWND, swap))
		{
			LOG(INFO) << "m_dx10.Initialize OK\n";
			goto HOOK_OK;
		}
		if (m_dx101.Initialize(hWND, swap))
		{
			LOG(INFO) << "m_dx101.Initialize OK\n";
			goto HOOK_OK;
		}
		if (m_dx11.Initialize(hWND, swap))
		{
			LOG(INFO) << "m_dx11.Initialize OK\n";
			goto HOOK_OK;
		}
	HOOK_OK:
		if (swap != NULL)
		{
			ULONG *vtable = *(ULONG**)swap.Ptr();
			if (!m_dxPresent.Initialize((FARPROC)*(vtable + (32 / 4)), (FARPROC)DX_DXGISwapPresent))
				return FALSE;
			if (!m_dxResizeBuffers.Initialize((FARPROC)*(vtable + (52 / 4)), (FARPROC)DX_DXGISwapResizeBuffers))
				return FALSE;
			if (!m_dxPresent.BeginDetour())
				return FALSE;
			if (!m_dxResizeBuffers.BeginDetour())
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}
	void DXGICapture::Release()
	{
		if (m_bDX10)
		{
			g_dx10.Release();
		}
		if (m_bDX101)
		{
			g_dx101.Release();
		}
		if (m_bDX11)
		{
			g_dx11.Release();
		}
	}
}