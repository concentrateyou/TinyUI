#pragma once
#include "D3DSystem.h"
#include "D3D10Texture.h"
#include "IO/TinySharedMemory.h"
using namespace TinyUI::IO;

namespace D3D
{
#define TEXTURE_MEMORY    TEXT("Local\\TextureMemory")
	typedef struct tagSharedCapture
	{
		UINT		CaptureType;
		DWORD		Format;
		UINT		Width;
		UINT		Height;
		BOOL		bFlip;
		UINT		Pitch;
		DWORD		MapSize;
		HWND		HwndCapture;
	}SharedCapture;//共享的捕获参数
	typedef struct tagCaptureEntry
	{
		UINT		CaptureType;
		DWORD		Format;
		UINT		Width;
		UINT		Height;
		BOOL		bFlip;
		UINT		Pitch;
		UINT		MapID;
		DWORD		MapSize;
		HWND		HwndCapture;
	}SharedTexture;
#pragma pack(push, 8)

	typedef struct tagSharedTextureData
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
	}SharedTextureData;

#pragma pack(pop)

	class CSharedTextureCapture
	{
	public:
		CSharedTextureCapture();
		~CSharedTextureCapture();
		BOOL Initialize();
	private:
		CD3D10Texture*		m_pSharedTexture;
		TinySharedMemory	m_textureMemery;
		HWND				m_hWNDTarget;
	};
}

