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
#pragma pack(push, 8)
	typedef struct tagSharedTexture
	{
		LONGLONG    FrameTime;
		HANDLE      TextureHandle;
	}SharedTexture;
#pragma pack(pop)

	class CSharedTextureCapture
	{
	public:
		explicit CSharedTextureCapture(CD3DSystem& system);
		~CSharedTextureCapture();
		BOOL Initialize();
	private:
		CD3D10Texture		sharedTexture;
		CD3D10Texture		cpoyTexture;
		TinySharedMemory	m_textureMemery;
		HWND				m_hWNDTarget;
	};
}

