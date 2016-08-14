#include "stdafx.h"
#include "VideoCommon.h"
#include "ScopedMediaType.h"

namespace Media
{
	ScopedMediaType::ScopedMediaType()
		:m_mediaType(NULL)
	{

	}
	ScopedMediaType::~ScopedMediaType()
	{
		DeleteMediaType(m_mediaType);
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
		::FreeMediaType(*m_mediaType);
	}
	AM_MEDIA_TYPE** ScopedMediaType::Receive()
	{
		return &m_mediaType;
	}
}
