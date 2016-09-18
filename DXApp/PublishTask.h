#pragma once
#include "DXFramework.h"
#include "Task.h"
using namespace TinyUI::IO;

class PublishTask : public Task
{
public:
	PublishTask(Closure& callback);
	PublishTask(Closure&& callback);
	~PublishTask();
};

