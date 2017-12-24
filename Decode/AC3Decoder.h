#pragma once
#include "Common.h"
#include "faad.h"
using namespace TinyUI;

namespace Decode
{
	class AC3Decoder
	{
		DISALLOW_COPY_AND_ASSIGN(AC3Decoder)
	public:
		AC3Decoder();
		virtual ~AC3Decoder();
	};

}
