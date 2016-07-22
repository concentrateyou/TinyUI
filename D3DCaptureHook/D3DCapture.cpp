#include "stdafx.h"
#include "D3DCapture.h"

typedef IDirect3D9* (WINAPI*D3D9CREATEPROC)(UINT);
typedef HRESULT(WINAPI*D3D9CREATEEXPROC)(UINT, IDirect3D9Ex**);

namespace D3D
{
	D3D9Capture::D3D9Capture()
		:m_hInstance(FALSE)
	{

	}
	D3D9Capture::~D3D9Capture()
	{

	}

	BOOL D3D9Capture::Initialize(HWND hWND)
	{
		HRESULT hRes = S_OK;
		CHAR szD3DPath[MAX_PATH];
		if (FAILED(hRes = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szD3DPath)))
			return FALSE;
		strcat_s(szD3DPath, MAX_PATH, TEXT("\\d3d9.dll"));
		m_hInstance = GetModuleHandle(szD3DPath);
		if (!m_hInstance)
			return FALSE;
		D3D9CREATEEXPROC d3d9CreateEx = (D3D9CREATEEXPROC)GetProcAddress(m_hInstance, "Direct3DCreate9Ex");
		if (!d3d9CreateEx)
			return FALSE;
		CComPtr<IDirect3D9Ex> d3d9ex;
		if (FAILED(hRes = (*d3d9CreateEx)(D3D_SDK_VERSION, &d3d9ex)))
			return FALSE;
		D3DPRESENT_PARAMETERS pp;
		ZeroMemory(&pp, sizeof(pp));
		pp.Windowed = 1;
		pp.SwapEffect = D3DSWAPEFFECT_FLIP;
		pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		pp.BackBufferCount = 1;
		pp.hDeviceWindow = hWND;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		if (FAILED(hRes = d3d9ex->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, hWND, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_NOWINDOWCHANGES, &pp, NULL, &m_device)))
			return FALSE;
		//获得虚表指针
		ULONG *vtable = *(ULONG**)m_device.Ptr();
		return TRUE;
	}

	HRESULT STDMETHODCALLTYPE D3D9Capture::D3D9EndScene(IDirect3DDevice9 *device)
	{
		D3D9Capture* _this = reinterpret_cast<D3D9Capture*>(device);
		if (!_this) return S_FALSE;
		return S_OK;
	}
}