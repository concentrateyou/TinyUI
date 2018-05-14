#pragma once
#include "Common/TinyCommon.h"
#include "IO/TinyFixedAlloc.h"
#include "Media/TinyMedia.h"
#include "dynlink_nvcuvid.h" 
#include "dynlink_cuda.h"   
#include "dynlink_cudaD3D9.h"
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;


namespace NVCodec
{
	BOOL checkCudaCapabilitiesDRV(INT majorV, INT minorV, INT devID);
}