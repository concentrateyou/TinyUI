#pragma once
#include <strmif.h>

namespace DShow
{
	class ScopedMediaType
	{
	public:
		ScopedMediaType(AM_MEDIA_TYPE* mediaType = NULL);
		ScopedMediaType(ScopedMediaType&&);
		~ScopedMediaType();
		AM_MEDIA_TYPE*	operator->();
		AM_MEDIA_TYPE*	Ptr();
		AM_MEDIA_TYPE** Receive();
		void Release();
	private:
		AM_MEDIA_TYPE* m_mediaType;
	};
}
