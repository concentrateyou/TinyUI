#pragma once
#include "DShowCommon.h"

namespace Media
{
	class AudioCaptureParam : public CaptureParam
	{
	public:
		AudioCaptureParam();
		virtual ~AudioCaptureParam();
		GUID MediaType() OVERRIDE;
		BOOL IsValid() const OVERRIDE;
		string ToString() const OVERRIDE;
	};
}
