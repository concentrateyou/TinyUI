#include "../stdafx.h"
#include <process.h>
#include "TinyD3D10Device.h"

namespace TinyUI
{
	namespace D3D
	{
		BOOL TinyD3D10Device::Initialize(HWND hWND, INT cx, INT cy)
		{
			TinyComPtr<IDXGIFactory1> factory;
			HRESULT hRes = S_OK;
			TinyComPtr<IDXGIAdapter1> adapter;
			if (FAILED(hRes = CreateDXGIFactory1(IID_IDXGIFactory1, (void**)&factory)))
			{
				return FALSE;
			}
			if (FAILED(factory->EnumAdapters1(0, &adapter)))
			{
				return FALSE;
			}
			DXGI_SWAP_CHAIN_DESC dscd;
			ZeroMemory(&dscd, sizeof(dscd));
			dscd.BufferCount = 1;
			dscd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			dscd.BufferDesc.RefreshRate.Numerator = 60;
			dscd.BufferDesc.RefreshRate.Denominator = 1;
			dscd.BufferDesc.Width = cx;
			dscd.BufferDesc.Height = cy;
			dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			dscd.OutputWindow = hWND;
			dscd.SampleDesc.Count = 1;
			dscd.Windowed = TRUE;
			dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			hRes = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &dscd, &m_swap, &m_d3d);
			if (FAILED(hRes))
				hRes = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT | D3D10_CREATE_DEVICE_DEBUG, D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &dscd, &m_swap, &m_d3d);
			if (FAILED(hRes))
				return FALSE;
			TinyComPtr<ID3D10Texture2D> backBuffer;
			hRes = m_swap->GetBuffer(0, IID_ID3D10Texture2D, (void**)&backBuffer);
			if (FAILED(hRes))
				return FALSE;
			hRes = m_d3d->CreateRenderTargetView(backBuffer, NULL, &m_renderView);
			if (FAILED(hRes))
				return FALSE;


			return TRUE;
		}
		void TinyD3D10Device::SetViewports(INT x, INT y, INT cx, INT cy)
		{
			ASSERT(m_d3d);
			D3D10_VIEWPORT vp;
			vp.Width = cx;
			vp.Height = cy;
			vp.MinDepth = 0.0F;
			vp.MaxDepth = 1.0F;
			vp.TopLeftX = x;
			vp.TopLeftY = y;
			m_d3d->RSSetViewports(1, &vp);
		}
		BOOL TinyD3D10Device::Render()
		{
			ASSERT(m_d3d);
			D3DXCOLOR clearColor(0, 0, 0, 0);
			m_d3d->ClearRenderTargetView(m_renderView, clearColor);
			m_swap->Present(0, 0);
			return TRUE;
		}
		ID3D10Device1*	TinyD3D10Device::GetDevice()
		{
			return m_d3d;
		}
		BOOL TinyD3D10Device::LoadTexture(LPCSTR pzFile, ID3D10Texture2D** texture2D)
		{
			ASSERT(m_d3d);
			TinyComPtr<ID3D10Resource> resource;
			if (FAILED(D3DX10CreateTextureFromFile(m_d3d, pzFile, NULL, NULL, &resource, NULL)))
			{
				return FALSE;
			}
			if (FAILED(resource->QueryInterface(__uuidof(ID3D10Texture2D), (LPVOID*)texture2D)))
			{
				return FALSE;
			}
			return TRUE;
		}
		BOOL TinyD3D10Device::GetResourceView(ID3D10Texture2D* texture2D, ID3D10ShaderResourceView **resourceView)
		{
			ASSERT(m_d3d);
			if (!texture2D)
				return FALSE;
			D3D10_TEXTURE2D_DESC desc;
			texture2D->GetDesc(&desc);
			D3D10_SHADER_RESOURCE_VIEW_DESC srvdesc;
			ZeroMemory(&srvdesc, sizeof(srvdesc));
			srvdesc.Format = desc.Format;
			srvdesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
			srvdesc.Texture2D.MipLevels = desc.MipLevels;
			return m_d3d->CreateShaderResourceView(texture2D, &srvdesc, resourceView) == S_OK;
		}
	}
}
