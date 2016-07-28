#include "stdafx.h"
#include "SharedTextureCapture.h"

namespace D3D
{
	CSharedTextureCapture::CSharedTextureCapture(CD3D10Device& system)
		:m_hWNDTarget(NULL),
		m_sharedTexture(system)
	{

	}

	CSharedTextureCapture::~CSharedTextureCapture()
	{

	}

	BOOL CSharedTextureCapture::Initialize()
	{
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
		if (!m_sharedTexture.CreateTexture(pTexture->TextureHandle))
		{
			return FALSE;
		}
		return TRUE;
	}
	CD3D10Texture* CSharedTextureCapture::GetSharedTexture()
	{
		return &m_sharedTexture;
	}
}