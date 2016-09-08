#pragma once
#include "DXFramework.h"
#include "BaseWorker.h"
using namespace TinyUI::IO;

class RenderWorker : public BaseWorker
{
public:
	RenderWorker(Closure& callback);
	~RenderWorker();
};

