#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "common/common_utils.h"
#include <vector>
#include <dxgi.h>
using namespace TinyUI;

namespace QSV
{
	typedef struct tagQSVParam
	{
		WORD wCX;
		WORD wCY;
		WORD wFPS;
		WORD wKbps;
		WORD wMaxKbps;
		WORD wAccuracy;
		WORD wConvergence;
		WORD wQPI;
		WORD wQPB;
		WORD wQPP;
		WORD wRC;
		WORD wKeyPerSec;//ÿ��ؼ�֡��
		WORD wbFrames;//b֡��
		WORD wAsyncDepth;
		WORD wTargetUsage;
	}QSVParam;
}

