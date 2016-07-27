#include "stdafx.h"
#include "SharedTextureCapture.h"

namespace D3D
{
	CSharedTextureCapture::CSharedTextureCapture(CD3DSystem& system)
		:m_hWNDTarget(NULL),
		sharedTexture(&system),
		cpoyTexture(&system)
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
		if (!sharedTexture.CreateTexture(pTexture->TextureHandle))
		{
			return FALSE;
		}
		return TRUE;
	}
}