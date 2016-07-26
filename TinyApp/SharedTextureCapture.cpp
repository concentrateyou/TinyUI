#include "stdafx.h"
#include "SharedTextureCapture.h"

namespace D3D
{
	CSharedTextureCapture::CSharedTextureCapture()
		:m_hWNDTarget(NULL)
	{

	}

	CSharedTextureCapture::~CSharedTextureCapture()
	{

	}

	BOOL CSharedTextureCapture::Initialize()
	{
		INT size = sizeof(SharedTextureData);
		if (!m_textureMemery.Open(TEXTURE_MEMORY, FALSE))
		{
			return FALSE;
		}
		if (!m_textureMemery.Map(0, size))
		{
			return FALSE;
		}
		return TRUE;
	}
}