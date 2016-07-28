#pragma once
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
		SIZE		Size;
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
		explicit CSharedTextureCapture(CD3D10Device& system);
		~CSharedTextureCapture();
		BOOL Initialize();
		CD3D10Texture*  GetSharedTexture();
		CD3D10Texture*	GetCopyTexture();
	private:
		CD3D10Texture		m_sharedTexture;
		TinySharedMemory	m_textureMemery;
		HWND				m_hWNDTarget;
	};
}

