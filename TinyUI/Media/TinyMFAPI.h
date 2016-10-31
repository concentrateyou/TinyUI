#pragma once
#include "TinyMedia.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyMFAPI
		{
		public:
			static HRESULT GetAudioDecoder(const GUID& subtype, IMFTransform **ppDecoder);
			static HRESULT GetAudioEncoder(const GUID& subtype, IMFTransform **ppEncoder);
			static HRESULT GetVideoDecoder(const GUID& subtype, IMFTransform **ppDecoder);
			static HRESULT GetVideoEncoder(const GUID& subtype, IMFTransform **ppEncoder);
			static HRESULT ConvertAudioType(const BYTE* pFromType, const BYTE* pToType);
		};
	};
}
