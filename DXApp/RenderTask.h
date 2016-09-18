#pragma once
#include "DXFramework.h"
#include "Task.h"
using namespace TinyUI::IO;

class RenderTask : public Task
{
public:
	RenderTask(Closure& callback);
	RenderTask(Closure&& callback);
	~RenderTask();
};

