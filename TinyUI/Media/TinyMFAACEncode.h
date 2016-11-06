#pragma once
#include "TinyMedia.h"
#include "TinyWave.h"

namespace TinyUI
{
	namespace Media
	{
		class TinyMFAACEncode
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMFAACEncode)
		public:
			TinyMFAACEncode();
			~TinyMFAACEncode();
		public:
			BOOL Open();
			BOOL Encode();
			BOOL Close();
		};
	};
}
