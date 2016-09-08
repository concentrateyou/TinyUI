#include "stdafx.h"
#include "PublishWorker.h"
#include "DXGraphics.h"

#define PUBLISH_FINISH_EVENT     TEXT("PUBLISH_FINISH")

PublishWorker::PublishWorker(Closure& callback)
	: BaseWorker(PUBLISH_FINISH_EVENT, callback)
{
}


PublishWorker::~PublishWorker()
{
}
