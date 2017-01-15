#pragma once
#include "DXFramework.h"
#include "DX11.h"

namespace DXFramework
{
#ifndef WIDTHBYTES
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8) 
#endif
	/// <summary>
	/// 纹理
	/// </summary>
	class DX11Texture
	{
		DISALLOW_COPY_AND_ASSIGN(DX11Texture)
	public:
		DX11Texture();
		~DX11Texture();
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
		BOOL IsValid() const;
		TinySize GetSize();
	protected:
		BOOL									m_bCompatible;
		TinyComPtr<IDXGISurface1>				m_surface;
		TinyComPtr<ID3D11Texture2D>				m_texture2D;
		TinyComPtr<ID3D11ShaderResourceView>	m_resourceView;
	};

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
#pragma pack(push, 8)
	/// <summary>
	/// 共享纹理数据
	/// </summary>
	typedef struct tagSharedTextureDATA
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
	}SharedTextureDATA;
#pragma pack(pop)
}

