#pragma once
#include "Control/TinyControl.h"
#include "Control/TinyMenu.h"
#include "RenderTask.h"
#include "RTMPReader.h"
using namespace Decode;

namespace MShow
{
	class MRenderTask : public TinyTaskBase
	{
	public:
		MRenderTask();
		~MRenderTask();
		BOOL Initialize(DXWindow* pWindow, INT cx, INT cy);
		BOOL Submit(Closure&& callback) OVERRIDE;
	private:
		RTMPReader	m_reader;
		DX2D		m_dx;
	};
}



