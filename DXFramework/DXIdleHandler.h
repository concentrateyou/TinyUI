#pragma once
#include "DXFramework.h"

namespace DXFramework
{
	class DXIdleHandler : public TinyIdleHandler
	{
	public:
		DXIdleHandler();
		~DXIdleHandler();
		BOOL OnIdle() OVERRIDE;
	};
}



