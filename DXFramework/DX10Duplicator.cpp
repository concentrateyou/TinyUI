#include "stdafx.h"
#include "DX10Duplicator.h"

namespace DXFramework
{
	DX10Duplicator::DX10Duplicator()
		:m_handle(NULL)
	{
	}

	DX10Duplicator::~DX10Duplicator()
	{
	}
	UINT DX10Duplicator::GetOutputs(DX10& dx10) const
	{
		UINT count = 0;
		HRESULT hRes = S_OK;
		TinyComPtr<IDXGIDevice> dxgi;
		hRes = dx10.GetD3D()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi);
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
	BOOL DX10Duplicator::Initialize(DX10& dx10, UINT index)
	{
		HRESULT hRes = S_OK;
		TinyComPtr<IDXGIDevice> dxgi;
		hRes = dx10.GetD3D()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi);
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
		hRes = output1->DuplicateOutput(dx10.GetD3D(), &m_duplication);
		if (hRes != S_OK)
			return FALSE;
		DXGI_OUTDUPL_DESC desc = { 0 };
		m_duplication->GetDesc(&desc);
		D3D10_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = desc.ModeDesc.Width;
		textureDesc.Height = desc.ModeDesc.Height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = desc.ModeDesc.Format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		textureDesc.Usage = D3D10_USAGE_DEFAULT;
		textureDesc.MiscFlags = D3D10_RESOURCE_MISC_SHARED;//必须是BGRA格式
		TinyComPtr<ID3D10Texture2D> texture2D;
		if (FAILED(hRes = dx10.GetD3D()->CreateTexture2D(&textureDesc, NULL, &texture2D)))
			return FALSE;
		if (FAILED(hRes = texture2D->QueryInterface(__uuidof(ID3D10Resource), (void**)&m_resource)))
			return FALSE;
		TinyComPtr<IDXGIResource> resource;
		if (FAILED(hRes = texture2D->QueryInterface(__uuidof(IDXGIResource), (void**)&resource)))
			return FALSE;
		if (FAILED(hRes = resource->GetSharedHandle(&m_handle)))
			return FALSE;
		return TRUE;
	}
	BOOL DX10Duplicator::AcquireNextFrame(DX10& dx10, UINT timeout)
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
		TinyComPtr<ID3D10Texture2D> texture2D;
		hRes = resource->QueryInterface(&texture2D);
		if (hRes != S_OK)
			return FALSE;
		dx10.GetD3D()->CopyResource(m_resource, texture2D.Ptr());
		hRes = m_duplication->ReleaseFrame();
		if (hRes != S_OK)
			return FALSE;
		return TRUE;
	}
	HANDLE	DX10Duplicator::GetSharedHandle() const
	{
		return m_handle;
	}
}
