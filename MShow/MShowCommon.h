#pragma once
#include <vector>
#include "malloc.h"
#include "rtmp.h"
#include "Common/TinyLogging.h"
#include "Common/TinyCallback.h"
#include "IO/TinyIO.h"
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingBuffer.h"
#include "IO/TinyFixedAlloc.h"
#include "Control/TinyControl.h"
#include "Control/TinyTabControl.h"
#include "Control/TinyButton.h"
#include "Media/TinyMedia.h"
#include "Media/TinySoundPlayer.h"
#include "Media/TinySoundCapture.h"
#include "Common/TinyTime.h"
#include "Common.h"
#include "x264Encode.h"
#include "AACEncode.h"
using namespace std;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;
using namespace Encode;
using namespace Decode;

