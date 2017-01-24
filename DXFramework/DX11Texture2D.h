#pragma once
#include "DXFramework.h"
#include "DX11.h"

namespace DXFramework
{
#ifndef WIDTHBYTES
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8) 
#endif
	/// <summary>
	/// 2D纹理
	/// </summary>
	class DX11Texture2D
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Texture2D)
	public:
		DX11Texture2D();
		virtual ~DX11Texture2D();
		BOOL Create(DX11& dx11, INT cx, INT cy, const BYTE* bits);
		BOOL Save(DX11& dx11, const CHAR* pzFile, D3DX11_IMAGE_FILE_FORMAT dxgi);
		BOOL Load(DX11& dx11, const BYTE* bits, DWORD dwSize);
		BOOL Load(DX11& dx11, HANDLE hResource);
		BOOL Load(DX11& dx11, const CHAR* pzFile);
		BOOL CreateCompatible(DX11& dx11, INT cx, INT cy, const BYTE* bits);
		void Destory();
		BOOL IsCompatible() const;
		BOOL GetDC(HDC& hDC);
		BOOL ReleaseDC();
		ID3D11Texture2D* GetTexture2D() const;
		ID3D11ShaderResourceView* GetSRView() const;
		BOOL IsEmpty() const;
		TinySize GetSize();
	protected:
		BOOL									m_bCompatible;
		TinyComPtr<IDXGISurface1>				m_surface;
		TinyComPtr<ID3D11Texture2D>				m_texture2D;
		TinyComPtr<ID3D11ShaderResourceView>	m_resourceView;
	};

#define CAPTURETYPE_MEMORY      1
#define CAPTURETYPE_SHAREDTEX   2

#pragma pack(push, 8)
	typedef struct tagSharedCaptureDATA
	{
		UINT		CaptureType;
		DWORD		Format;
		SIZE		Size;
		BOOL		bFlip;
		UINT		Pitch;
		DWORD		MapSize;
		HWND		HwndCapture;
	}SharedCaptureDATA;
	/// <summary>
	/// 共享纹理数据
	/// </summary>
	typedef struct tagSharedTextureDATA
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
		DWORD       Texture1Offset;
		DWORD		Texture2Offset;
		DWORD		CurrentIndex;
	}SharedTextureDATA;
#pragma pack(pop)
}

