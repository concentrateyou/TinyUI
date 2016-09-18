#include "stdafx.h"
#include "RenderTask.h"
#include "DXGraphics.h"

RenderTask::RenderTask(Closure& callback)
	:Task(callback)
{

}

RenderTask::RenderTask(Closure&& callback)
	: Task(std::move(callback))
{

}


RenderTask::~RenderTask()
{
}
