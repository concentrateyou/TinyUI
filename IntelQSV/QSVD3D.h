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
}



