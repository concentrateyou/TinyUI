#pragma once
#include "DX10Image2D.h"
#include <dxgi1_2.h>

namespace DXFramework
{
	class DX10Duplicator
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Duplicator)
	public:
		DX10Duplicator();
		~DX10Duplicator();
		UINT GetOutputs(DX10& dx11) const;
		BOOL Initialize(DX10& dx11, UINT index);
		BOOL AcquireNextFrame(DX10& dx11, UINT timeout);
		HANDLE GetSharedHandle() const;
	private:
		HANDLE								m_handle;
		TinyComPtr<ID3D10Resource>			m_resource;
		TinyComPtr<IDXGIOutputDuplication>	m_duplication;
	};
}



