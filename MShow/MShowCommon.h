#pragma once
#include <vector>
#include "malloc.h"
#include "rtmp.h"
#include "Common/TinyLogging.h"
#include "Common/TinyCallback.h"
#include "Common/TinyTime.h"
#include "IO/TinyIO.h"
#include "IO/TinyTask.h"
#include "IO/TinyRingBuffer.h"
#include "IO/TinyFixedAlloc.h"
#include "Media/TinyMedia.h"
#include "Media/TinySoundPlayer.h"
#include "Media/TinySoundCapture.h"
#include "Media/TinyXAudio.h"
#include "Control/TinyControl.h"
#include "Control/TinyTabControl.h"
#include "Control/TinyButton.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualDocument.h"
#include "Windowless/TinyVisualNative.h"
#include "Windowless/TinyVisualComboBox.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Windowless/TinyVisualButton.h"
#include "Windowless/TinyVisualCaption.h"
#include "Windowless/TinyVisualLabel.h"
#include "Windowless/TinyVisualPanel.h"
#include "Windowless/TinyVisualScrollBar.h"
#include "Windowless/TinyVisualTab.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Windowless/TinyVisualList.h"
#include "Windowless/TinyVisualAnimation.h"
#include "json/json.h"
#include "Common.h"
#include "x264Encode.h"
#include "AACEncode.h"
using namespace std;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;
using namespace Encode;
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