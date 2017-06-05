#include "stdafx.h"
#include "MRenderTask.h"

namespace MShow
{
	MRenderTask::MRenderTask()
	{

	}
	MRenderTask::~MRenderTask()
	{

	}
	BOOL MRenderTask::Initialize(DXWindow* pWindow, INT cx, INT cy)
	{

	}
	BOOL MRenderTask::Submit(Closure&& callback)
	{

		return TinyTaskBase::Submit(callback);
	}
}