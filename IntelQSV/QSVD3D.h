#pragma once
#include "QSVCommon.h"

namespace QSV
{
	class QSVD3D
	{
	public:
		virtual mfxStatus Initialize(mfxHDL hWindow, UINT adapter) = 0;
		virtual mfxHDL    GetHandle(mfxHandleType type) = 0;
		virtual mfxStatus Reset() = 0;
		virtual void      Close() = 0;
	};
	/// <summary>
	/// D3D9
	/// </summary>
	class QSVD3D9 : public QSVD3D
	{
		DISALLOW_COPY_AND_ASSIGN(QSVD3D9)
	public:
		QSVD3D9();
		virtual ~QSVD3D9();
	public:
		mfxStatus Initialize(mfxHDL hWindow,UINT adapter) OVERRIDE;
		mfxHDL GetHandle(mfxHandleType type) OVERRIDE;
		mfxStatus Reset() OVERRIDE;
		void Close() OVERRIDE;
	public:
		DWORD GetUsage() const;
	private:
		UINT								m_resetToken;
		HANDLE								m_handle;
		D3DPRESENT_PARAMETERS				m_d3dPP;
		TinyComPtr<IDirect3D9Ex>			m_d3d9;
		TinyComPtr<IDirect3DDevice9Ex>		m_d3dd9;
		TinyComPtr<IDirect3DDeviceManager9>	m_manager;
	};
	/// <summary>
	/// D3D11
	/// </summary>
	class QSVD3D11 : public QSVD3D
	{
		DISALLOW_COPY_AND_ASSIGN(QSVD3D11)
	public:
		QSVD3D11();
		virtual ~QSVD3D11();
	public:
		mfxStatus Initialize(mfxHDL hWindow, UINT adapter) OVERRIDE;
		mfxHDL GetHandle(mfxHandleType type) OVERRIDE;
		mfxStatus Reset() OVERRIDE;
		void Close() OVERRIDE;
	protected:
		TinyComPtr<ID3D11Device>         m_pD3D11Device;
		TinyComPtr<ID3D11DeviceContext>  m_pD3D11Ctx;
		TinyComQIPtr<IDXGIAdapter>       m_pAdapter;
		TinyComPtr<IDXGIFactory2>        m_pDXGIFactory;
		TinyComQIPtr<ID3D11VideoDevice>  m_pDX11VideoDevice;
		TinyComQIPtr<ID3D11VideoContext> m_pVideoContext;
	};
}



