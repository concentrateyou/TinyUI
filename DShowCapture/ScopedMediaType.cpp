#include "stdafx.h"
#include "DShowCommon.h"
#include "ScopedMediaType.h"

namespace DShow
{
	ScopedMediaType::ScopedMediaType(AM_MEDIA_TYPE* mediaType)
		: m_mediaType(mediaType)
	{

	}
	ScopedMediaType::~ScopedMediaType()
	{
		if (m_mediaType != NULL)
		{
			DeleteMediaType(m_mediaType);
			m_mediaType = NULL;
		}
	}
	ScopedMediaType::ScopedMediaType(ScopedMediaType&& mediaType)
		:m_mediaType(mediaType.m_mediaType)
	{
		mediaType.m_mediaType = NULL;
	}
	AM_MEDIA_TYPE* ScopedMediaType::operator->()
	{
		return m_mediaType;
	}
	AM_MEDIA_TYPE* ScopedMediaType::Ptr()
	{
		return m_mediaType;
	}
	void ScopedMediaType::Release()
	{
		if (m_mediaType != NULL)
		{
			::FreeMediaType(*m_mediaType);
			m_mediaType = NULL;
		}
	}
	AM_MEDIA_TYPE** ScopedMediaType::Receive()
	{
		return &m_mediaType;
	}
}
