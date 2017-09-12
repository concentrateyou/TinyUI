#include "stdafx.h"
#include "DX11Duplicator.h"

namespace DXFramework
{
	/*DX11Duplicator::DX11Duplicator()
		:m_handle(NULL)
	{
	}

	DX11Duplicator::~DX11Duplicator()
	{
	}
	UINT DX11Duplicator::GetOutputs(DX11& dx11) const
	{
		UINT count = 0;
		HRESULT hRes = S_OK;
		TinyComPtr<IDXGIDevice> dxgi;
		hRes = dx11.GetD3D()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IDXGIAdapter> adapter;
		hRes = dxgi->GetAdapter(&adapter);
		if (hRes != S_OK)
			return FALSE;
		for (;;)
		{
			TinyComPtr<IDXGIOutput> output;
			hRes = adapter->EnumOutputs(count, &output);
			if (hRes != S_OK)
				break;
			count++;
		}
		return count;

	}
	BOOL DX11Duplicator::Initialize(DX11& dx11, UINT index)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<IDXGIDevice> dxgi;
		hRes = dx11.GetD3D()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IDXGIAdapter> adapter;
		hRes = dxgi->GetAdapter(&adapter);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IDXGIOutput> output;
		hRes = adapter->EnumOutputs(index, &output);
		if (hRes != S_OK)
			return FALSE;
		TinyComPtr<IDXGIOutput1> output1;
		hRes = output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1);
		if (hRes != S_OK)
			return FALSE;
		hRes = output1->DuplicateOutput(dx11.GetD3D(), &m_duplication);
		if (hRes != S_OK)
			return FALSE;
		DXGI_OUTDUPL_DESC desc = { 0 };
		m_duplication->GetDesc(&desc);
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = desc.ModeDesc.Width;
		textureDesc.Height = desc.ModeDesc.Height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = desc.ModeDesc.Format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
		TinyComPtr<ID3D11Texture2D> texture2D;
		if (FAILED(hRes = dx11.GetD3D()->CreateTexture2D(&textureDesc, NULL, &texture2D)))
			return FALSE;
		if (FAILED(hRes = texture2D->QueryInterface(__uuidof(ID3D11Resource), (void**)&m_resource)))
			return FALSE;
		TinyComPtr<IDXGIResource> resource;
		if (FAILED(hRes = texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
			return FALSE;
		if (FAILED(hRes = resource->GetSharedHandle(&m_handle)))
			return FALSE;
		return TRUE;
	}
	BOOL DX11Duplicator::AcquireNextFrame(DX11& dx11, UINT timeout)
	{
		if (!m_duplication)
			return FALSE;
		DXGI_OUTDUPL_FRAME_INFO s;
		TinyComPtr<IDXGIResource> resource;
		HRESULT hRes = m_duplication->AcquireNextFrame(timeout, &s, &resource);
		if (hRes != S_OK)
			return FALSE;
		if (s.LastPresentTime.QuadPart <= 0)
			return FALSE;
		TinyComPtr<ID3D11Texture2D> texture2D;
		hRes = resource->QueryInterface(&texture2D);
		if (hRes != S_OK)
			return FALSE;
		dx11.GetImmediateContext()->CopyResource(m_resource, texture2D.Ptr());
		hRes = m_duplication->ReleaseFrame();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	HANDLE	DX11Duplicator::GetSharedHandle() const
	{
		return m_handle;
	}*/
}
