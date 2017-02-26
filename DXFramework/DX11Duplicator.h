#pragma once
#include "DX11Image2D.h"
#include <dxgi1_2.h>

namespace DXFramework
{
	class DX11Duplicator
	{
	public:
		DX11Duplicator();
		~DX11Duplicator();
		UINT GetOutputs(DX11& dx11) const;
		BOOL Initialize(DX11& dx11, UINT index);
		BOOL AcquireNextFrame(DX11& dx11, UINT timeout = 100);
		HANDLE GetSharedHandle() const;
	private:
		HANDLE								m_handle;
		TinyComPtr<ID3D11Resource>			m_resource;
		TinyComPtr<IDXGIOutputDuplication>	m_duplication;
	};
}



