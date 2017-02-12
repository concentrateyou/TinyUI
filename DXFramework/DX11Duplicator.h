#pragma once
#include "DX11Image.h"
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
		BOOL AcquireNextFrame(DX11& dx11, UINT timeout);
	private:
		DX11Image							m_image;
		TinyComPtr<IDXGIOutputDuplication>	m_duplication;
	};
}



