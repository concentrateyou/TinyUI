#pragma once
#include <d3d9.h>
#include <D3D10_1.h>
#include "D3DHook.h"
#include "D3DSemaphore.h"
#include "D3DThunk.h"
#include <string>
using namespace std;

namespace D3D
{
	/// <summary>
	/// D3D9
	/// </summary>
	class D3D9Capture
	{
	public:
		D3D9Capture();
		~D3D9Capture();
		BOOL Initialize(HWND hWND);
	private:
		static HRESULT STDMETHODCALLTYPE D3D9EndScene(IDirect3DDevice9 *device);
	private:
		CComPtr<IDirect3DDevice9Ex>	m_device;
		CLock						m_lock;
		HMODULE						m_hInstance;
		D3DAPIHook					m_d3d9EndScene;
		CHookThunk					m_d3d9EndSceneThunk;
	};
}
