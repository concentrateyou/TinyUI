#pragma once
#include "DXFramework.h"
#include "Task.h"
using namespace TinyUI::IO;

class EncodeTask  : public Task
{
public:
	EncodeTask(Closure& callback);
	~EncodeTask();
};

