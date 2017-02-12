#include "stdafx.h"
#include "DX11Duplicator.h"

namespace DXFramework
{
	DX11Duplicator::DX11Duplicator()
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
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIAdapter> adapter;
		hRes = dxgi->GetAdapter(&adapter);
		if (FAILED(hRes))
			return FALSE;
		for (;;)
		{
			TinyComPtr<IDXGIOutput> output;
			hRes = adapter->EnumOutputs(count, &output);
			if (FAILED(hRes))
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
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIAdapter> adapter;
		hRes = dxgi->GetAdapter(&adapter);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIOutput> output;
		hRes = adapter->EnumOutputs(index, &output);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<IDXGIOutput1> output1;
		hRes = output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1);
		if (FAILED(hRes))
			return FALSE;
		hRes = output1->DuplicateOutput(dx11.GetD3D(), &m_duplication);
		if (FAILED(hRes))
			return FALSE;
		DXGI_OUTDUPL_DESC desc = { 0 };
		m_duplication->GetDesc(&desc);
		return TRUE;
	}
	BOOL DX11Duplicator::AcquireNextFrame(DX11& dx11, UINT timeout)
	{
		if (!m_duplication)
			return FALSE;
		DXGI_OUTDUPL_FRAME_INFO s;
		TinyComPtr<IDXGIResource> resource;
		HRESULT hRes = m_duplication->AcquireNextFrame(timeout, &s, &resource);
		if (FAILED(hRes))
			return FALSE;
		TinyComPtr<ID3D11Texture2D> texture2D;
		hRes = resource->QueryInterface(&texture2D);
		if (FAILED(hRes))
			return FALSE;
		if (!m_image.Copy(dx11, texture2D))
			return FALSE;
		hRes = m_duplication->ReleaseFrame();
		if (FAILED(hRes))
			return FALSE;
		return TRUE;
	}
}
