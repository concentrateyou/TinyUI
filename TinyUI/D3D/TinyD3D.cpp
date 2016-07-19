#include "../stdafx.h"
#include "TinyD3D.h"

namespace TinyUI
{
	namespace D3D
	{
		BOOL D3DCapture::GetDevices()
		{
			TinyComPtr<IDXGIFactory1> gif1;
			HRESULT hRes = S_OK;
			if (SUCCEEDED(hRes = CreateDXGIFactory1(IID_IDXGIFactory1, (void**)&gif1)))
			{
				UINT index = 0;
				TinyComPtr<IDXGIAdapter1> gia1;
				while (gif1->EnumAdapters1(index++, &gia1) == S_OK)
				{
					DXGI_ADAPTER_DESC desc;
					if (SUCCEEDED(hRes = gia1->GetDesc(&desc)))
					{

					}
				}
			}
			return TRUE;
		}
	}
}
