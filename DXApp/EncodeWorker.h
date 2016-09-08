#pragma once
#include "DXFramework.h"
#include "BaseWorker.h"
using namespace TinyUI::IO;

class EncodeWorker  : public BaseWorker
{
public:
	EncodeWorker(Closure& callback);
	~EncodeWorker();
};

