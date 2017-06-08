#pragma once
#include "MShowCommon.h"

namespace MShow
{
	class MVideoTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MVideoTask)
	public:
		MVideoTask();
		virtual ~MVideoTask();
	};
}