#pragma once
#include <vector>
#include "malloc.h"
#include "rtmp.h"
#include "Common/TinyLogging.h"
#include "Common/TinyCallback.h"
#include "Common/TinyTime.h"
#include "IO/TinyIO.h"
#include "IO/TinyThread.h"
#include "IO/TinyRingBuffer.h"
#include "IO/TinyFixedAlloc.h"
#include "IO/TinyMsgQueue.h"
#include "Media/TinyMedia.h"
#include "Media/TinySoundPlayer.h"
#include "Media/TinyXAudio.h"
#include "Control/TinyControl.h"
#include "Common.h"
#include "AACDecoder.h"
#include "x264Decoder.h"
using namespace std;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;
using namespace Decode;

typedef struct tagSEARCH_ITEM
{
	string	szLogID;
	string	szProgramName;
	string	szProgramID;
	string	szPreviewURL;
	string	szBeginTime;
	string	szEndTime;
}SEARCH_ITEM;

#define  WM_FLV_PARSE_FAIL			(WM_USER + 0x0010)
#define  WM_VIDEO_X264_DECODE_FAIL	(WM_USER + 0x0011)
#define  WM_AUDIO_AAC_DECODE_FAIL	(WM_USER + 0x0012)
#define  WM_PLAY_RESUME				(WM_USER + 0x0013)