#pragma once
#include "DXFramework.h"
#include "DX10.h"

namespace DXFramework
{
#ifndef WIDTHBYTES
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8) 
#endif
	/// <summary>
	/// Œ∆¿Ì
	/// </summary>
	class DX10Texture
	{
		DISALLOW_COPY_AND_ASSIGN(DX10Texture)
	public:
		DX10Texture();
		~DX10Texture();
		BOOL CreateTexture(const DX10& dx10, INT cx, INT cy, DXGI_FORMAT dxgi, void *lpData);
		BOOL FillTexture(const BYTE* pBits, INT cx, INT cy);
		BOOL Save(const CHAR* pzFile, D3DX10_IMAGE_FILE_FORMAT dxgi);
		BOOL LoadTexture(const DX10& dx10, const BYTE* pBits, DWORD dwSize);
		BOOL LoadTexture(const DX10& dx10, HANDLE hResource);
		BOOL LoadTexture(const DX10& dx10, const CHAR* pzFile);
		TinyComPtr<IDXGISurface1> GetSurface();
		ID3D10Texture2D* GetTexture2D() const;
		ID3D10ShaderResourceView* GetSRView() const;
		BOOL IsValid() const;
		TinySize GetSize();
	private:
		TinyComPtr<ID3D10Texture2D>				m_texture2D;
		TinyComPtr<ID3D10ShaderResourceView>	m_resourceView;
	};

	typedef struct tagSharedCapture
	{
		UINT		CaptureType;
		DWORD		Format;
		SIZE		Size;
		BOOL		bFlip;
		UINT		Pitch;
		DWORD		MapSize;
		HWND		HwndCapture;
	}SharedCapture;
#pragma pack(push, 8)
	typedef struct tagSharedTexture
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
	}SharedTexture;
#pragma pack(pop)
	class SharedTextureCapture
	{
	public:
		SharedTextureCapture();
		~SharedTextureCapture();
		DX10Texture* GetSharedTexture();
		DX10Texture* LockTexture(const DX10& dx10);
		BOOL Initialize(const DX10& dx10, SharedCapture* sharedCapture);
	private:
		DX10Texture				m_sharedTexture;
		DX10Texture				m_texture;
		IO::TinySharedMemory	m_textureMemery;
		HWND					m_hWND;
	};
}

