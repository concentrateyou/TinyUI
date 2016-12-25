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
		BOOL CreateTexture(const DX11& dx11, INT cx, INT cy, DXGI_FORMAT dxgi, void *lpData);
		BOOL FillTexture(const DX11& dx11, const BYTE* data);
		BOOL Save(const DX11& dx11, const CHAR* pzFile, D3DX11_IMAGE_FILE_FORMAT dxgi);
		BOOL LoadTexture(const DX11& dx11, const BYTE* data, DWORD dwSize);
		BOOL LoadTexture(const DX11& dx11, HANDLE hResource);
		BOOL LoadTexture(const DX11& dx11, const CHAR* pzFile);
		TinyComPtr<IDXGISurface1> GetSurface();
		ID3D11Texture2D* GetTexture2D() const;
		ID3D11ShaderResourceView* GetSRView() const;
		BOOL IsValid() const;
		TinySize GetSize();
	private:
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

