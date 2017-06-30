#pragma once
#include "rtmp.h"
#include "DX11.h"
#include "DX11Element2D.h"
#include "DX11Image2D.h"
#include "DX11Graphics2D.h"
#include "Common/TinyCallback.h"
#include "IO/TinyIO.h"
#include "IO/TinyTaskBase.h"
#include "IO/TinyRingBuffer.h"
#include "Control/TinyControl.h"
#include "Control/TinyTabControl.h"
#include "Control/TinyButton.h"
#include "Media/TinySoundPlayer.h"
#include "Common/TinyTime.h"
#include "Common.h"
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;
using namespace DXFramework;
using namespace Decode;

typedef struct tagMediaTag
{
	DWORD	dwType;
	DWORD	dwFlag;
	DWORD	dwINC;
	DWORD	dwTime;
	DWORD	dwPTS;
	DWORD	dwDTS;
}MediaTag;

typedef struct tagSample
{
	MediaTag	mediaTag;
	LONG		size;
	BYTE*		bits;
}Sample;
