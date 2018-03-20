#pragma once
#include "GLFramework.h"

namespace GLFramework
{
	class GL
	{
		DISALLOW_COPY_AND_ASSIGN(GL)
	public:
		GL();
		virtual ~GL();
		BOOL Initialize();
		BOOL Render();
	};
}
