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
			//D3D10_RASTERIZER_DESC drd;
			//ZeroMemory(&drd, sizeof(drd));
			//drd.FillMode = D3D10_FILL_SOLID;
			//drd.CullMode = D3D10_CULL_NONE;
			//drd.FrontCounterClockwise = FALSE;
			//drd.DepthClipEnable = TRUE;
			//hRes = m_d3d->CreateRasterizerState(&drd, &m_rasterizerState);
			//if (FAILED(hRes))
			//	return FALSE;
			//m_d3d->RSSetState(m_rasterizerState);
			//D3D10_DEPTH_STENCIL_DESC ddsd;
			//ZeroMemory(&ddsd, sizeof(ddsd));
			//ddsd.DepthEnable = FALSE;
			//hRes = m_d3d->CreateDepthStencilState(&ddsd, &m_depthState);
			//if (FAILED(hRes))
			//	return FALSE;
			//m_d3d->OMSetDepthStencilState(m_depthState, 0);
			//D3D10_BLEND_DESC dbd;
			//ZeroMemory(&dbd, sizeof(dbd));
			//for (int i = 0; i < 8; i++)
			//{
			//	dbd.BlendEnable[i] = TRUE;
			//	dbd.RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL;
			//}
			//dbd.BlendOpAlpha = D3D10_BLEND_OP_ADD;
			//dbd.BlendOp = D3D10_BLEND_OP_ADD;
			//dbd.SrcBlendAlpha = D3D10_BLEND_ONE;
			//dbd.DestBlendAlpha = D3D10_BLEND_ZERO;
			//dbd.SrcBlend = D3D10_BLEND_ONE;
			//dbd.DestBlend = D3D10_BLEND_ZERO;
			//hRes = m_d3d->CreateBlendState(&dbd, &m_blendState);
			//if (FAILED(hRes))
			//	return FALSE;
			//	
			m_d3d->OMSetRenderTargets(1, &m_renderView, NULL);
			this->SetViewports(0, 0, cx, cy);

			//ID3D10Texture2D* texture2D = NULL;
			//this->LoadTexture(TEXT("D:\\test.jpg"), &texture2D);
			//this->GetResourceView(texture2D, &m_resourceView);
			//m_s.pTexture = m_resourceView;
			//m_s.TexCoord.x = 0;
			//m_s.TexCoord.y = 0;
			//m_s.TexSize.x = 1.0f;
			//m_s.TexSize.y = 1.0f;
			//m_s.TextureIndex = 0;
			//m_s.ColorModulate = D3DXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
			//D3DX10CreateSprite(m_d3d, 0, &m_sprite);
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
			m_sprite->Begin(D3DX10_SPRITE_SORT_TEXTURE);
			m_sprite->DrawSpritesBuffered(&m_s, 1);
			m_sprite->Flush();
			m_sprite->End();
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
