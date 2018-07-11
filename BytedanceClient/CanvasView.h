#pragma once
#include "Control/TinyControl.h"
using namespace TinyFramework;

namespace Bytedance
{
	class CanvasView : public TinyControl
	{
		DECLARE_DYNAMIC(CanvasView)
	public:
		CanvasView();
		virtual ~CanvasView();
		LPCSTR	RetrieveClassName() OVERRIDE;
		LPCSTR	RetrieveTitle() OVERRIDE;
		DWORD	RetrieveStyle() OVERRIDE;
		DWORD	RetrieveExStyle() OVERRIDE;
	};
}


