#include "stdafx.h"
#include "PublishTask.h"
#include "DXGraphics.h"

PublishTask::PublishTask(Closure& callback)
	: Task(callback)
{
}

PublishTask::PublishTask(Closure&& callback)
	: Task(callback)
{

}

PublishTask::~PublishTask()
{
}
