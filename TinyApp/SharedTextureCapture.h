#pragma once
#include "D3D10Texture.h"
#include "IO/TinySharedMemory.h"
using namespace TinyUI::IO;

namespace D3D
{
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
		explicit CSharedTextureCapture();
		~CSharedTextureCapture();
		CD3D10Texture*  GetSharedTexture();
		BOOL Initialize(CD3D10Device* device, SharedCapture* sharedCapture);
		CD3D10Texture* LockTexture(CD3D10Device* device);
	private:
		CD3D10Texture		m_sharedTexture;
		CD3D10Texture		m_copyTexture;
		TinySharedMemory	m_textureMemery;
		HWND				m_hWNDTarget;
	};
}

