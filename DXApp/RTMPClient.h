#pragma once
#include "DXFramework.h"
extern "C"
{
#include "rtmp.h"
#include "amf.h"
#include "log.h"
}
#pragma comment(lib,"librtmp.lib")

#define STREAM_CHANNEL_METADATA  0x03  
#define STREAM_CHANNEL_VIDEO     0x04  
#define STREAM_CHANNEL_AUDIO     0x05  

namespace DXApp
{

	class RTMPClient
	{
	public:
		RTMPClient();
		~RTMPClient();
	public:
		UINT GetTime();
		BOOL Connect(const TinyString& url);
		BOOL SendMetadata(INT cx, INT cy, INT frameRate, INT videoRate, const WAVEFORMATEX& wfx, INT audioRate);
		BOOL SendSPP(const vector<BYTE>& pps, const vector<BYTE>& sps, DWORD timestamp);
		BOOL SendAAC(BYTE* bits, INT size);
		BOOL SendVideo(BYTE* bits, INT size, DWORD timestamp);
		BOOL SendAudio(BYTE* bits, INT size, DWORD timestamp);
		BOOL Reconnect();
	private:
		RTMP*		m_pRTMP;
	};
}
