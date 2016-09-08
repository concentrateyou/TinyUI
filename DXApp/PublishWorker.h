#pragma once
#include "DXFramework.h"
#include "BaseWorker.h"
using namespace TinyUI::IO;

class PublishWorker : public BaseWorker
{
public:
	PublishWorker(Closure& callback);
	~PublishWorker();
};

