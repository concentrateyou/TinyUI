#include "stdafx.h"
#include "SharedTextureCapture.h"
#include "D3DCaptureSource.h"

namespace D3D
{
	CSharedTextureCapture::CSharedTextureCapture()
		:m_hWNDTarget(NULL)
	{

	}

	CSharedTextureCapture::~CSharedTextureCapture()
	{

	}

	BOOL CSharedTextureCapture::Initialize(CD3D10Device* device, SharedCapture* sharedCapture)
	{
		ASSERT(device);
		if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
		{
			return FALSE;
		}
		if (!m_textureMemery.Map(0, sizeof(SharedTexture)))
		{
			return FALSE;
		}
		SharedTexture* pTexture = reinterpret_cast<SharedTexture*>(m_textureMemery.Address());
		if (!pTexture)
		{
			return FALSE;
		}
		if (!m_sharedTexture.CreateTexture(device, pTexture->TextureHandle))
		{
			return FALSE;
		}
		if (!m_copyTexture.CreateTexture(device, sharedCapture->Size, (DXGI_FORMAT)sharedCapture->Format, NULL, FALSE, TRUE))
		{
			return FALSE;
		}
		return TRUE;
	}
	CD3D10Texture* CSharedTextureCapture::LockTexture(CD3D10Device* device)
	{
		ASSERT(device);
		device->CopyTexture(&m_copyTexture, &m_sharedTexture);
		return &m_copyTexture;
	}
	CD3D10Texture* CSharedTextureCapture::GetSharedTexture()
	{
		return &m_sharedTexture;
	}
}