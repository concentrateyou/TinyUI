#include "stdafx.h"
#include "DXGIGraphicsCapture.h"

namespace GraphicsCapture
{
	HRESULT STDMETHODCALLTYPE DX_DXGISwapPresent(IDXGISwapChain *swap, UINT syncInterval, UINT flags)
	{
		g_dxgi.m_present.EndDetour();
		TinyComPtr<IUnknown> unknow;
		if (SUCCEEDED(swap->GetDevice(__uuidof(IUnknown), (void**)&unknow)))
		{
			do
			{
				TinyComPtr<IUnknown> device;
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device), (void**)&device)))
				{
					g_dxgi.m_bDX10 = TRUE;
					if (g_dxgi.m_currentSwap == NULL)
					{
						g_dxgi.m_currentSwap = g_dx10.Setup(swap) ? swap : NULL;
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dxgi.m_currentSwap == swap)
					{
						g_dx10.Draw(swap, flags);
					}
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device1), (void**)&device)))
				{
					g_dxgi.m_bDX101 = TRUE;
					if (g_dxgi.m_currentSwap == NULL)
					{
						g_dxgi.m_currentSwap = g_dx101.Setup(swap) ? swap : NULL;
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dxgi.m_currentSwap == swap)
					{
						g_dx101.Draw(swap, flags);
					}
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D11Device), (void**)&device)))
				{
					g_dxgi.m_bDX11 = TRUE;
					if (g_dxgi.m_currentSwap == NULL)
					{
						g_dxgi.m_currentSwap = g_dx11.Setup(swap) ? swap : NULL;
					}
					if ((flags & DXGI_PRESENT_TEST) == 0 && g_dxgi.m_currentSwap == swap)
					{
						g_dx11.Draw(swap, flags);
					}
					break;
				}
			} while (0);
		}
		HRESULT hRes = swap->Present(syncInterval, flags);
		g_dxgi.m_present.BeginDetour();
		return hRes;
	}
	HRESULT STDMETHODCALLTYPE DX_DXGISwapResizeBuffers(IDXGISwapChain *swap, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT giFormat, UINT flags)
	{
		g_dxgi.m_resize.EndDetour();
		TinyComPtr<IUnknown> unknow;
		if (SUCCEEDED(swap->GetDevice(__uuidof(IUnknown), (void**)&unknow)))
		{
			do
			{
				TinyComPtr<IUnknown> device;
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device), (void**)&device)))
				{
					g_dxgi.m_bDX10 = TRUE;
					g_dx10.Reset();
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D10Device1), (void**)&device)))
				{
					g_dxgi.m_bDX101 = TRUE;
					g_dx101.Reset();
					break;
				}
				if (SUCCEEDED(unknow->QueryInterface(__uuidof(ID3D11Device), (void**)&device)))
				{
					g_dxgi.m_bDX11 = TRUE;
					g_dx11.Reset();
					break;
				}
			} while (0);
		}
		g_dxgi.m_currentSwap = NULL;
		HRESULT hRes = swap->ResizeBuffers(bufferCount, width, height, giFormat, flags);
		g_dxgi.m_resize.BeginDetour();
		return hRes;
	}

	DXGIGraphicsCapture::DXGIGraphicsCapture(DX10GraphicsCapture& dx10, DX101GraphicsCapture& dx101, DX11GraphicsCapture& dx11)
		:m_bDX10(FALSE),
		m_bDX101(FALSE),
		m_bDX11(FALSE),
		m_dx10(dx10),
		m_dx101(dx101),
		m_dx11(dx11),
		m_currentSwap(NULL)
	{

	}
	DXGIGraphicsCapture::~DXGIGraphicsCapture()
	{

	}
	BOOL DXGIGraphicsCapture::Initialize(HWND hWND)
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
			if (!m_present.Initialize((FARPROC)*(vtable + (32 / 4)), (FARPROC)DX_DXGISwapPresent))
				return FALSE;
			if (!m_resize.Initialize((FARPROC)*(vtable + (52 / 4)), (FARPROC)DX_DXGISwapResizeBuffers))
				return FALSE;
			if (!m_present.BeginDetour())
				return FALSE;
			if (!m_resize.BeginDetour())
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}
	BOOL DXGIGraphicsCapture::hookable()
	{
		return	!m_present.IsEmpty() && !m_resize.IsEmpty();
	}
	void DXGIGraphicsCapture::Reset()
	{
		if (m_bDX10)
		{
			g_dx10.Reset();
		}
		if (m_bDX101)
		{
			g_dx101.Reset();
		}
		if (m_bDX11)
		{
			g_dx11.Reset();
		}
	}
}