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

class RTMPPublisher
{
public:
	RTMPPublisher();
	~RTMPPublisher();
public:
	UINT GetTime();
	BOOL Connect(const TinyString& url);
	BOOL SendMetadataPacket(INT cx, INT cy, INT fps, INT rate);
	BOOL SendSPSPPSPacket(const vector<BYTE>& pps, const vector<BYTE>& sps);
	BOOL SendAACPacket(BYTE* bits, INT size);
	BOOL SendVideoPacket(BYTE* bits, INT size, DWORD timeoffset);
	BOOL SendAudioPacket(BYTE* bits, INT size, DWORD timeoffset);
	BOOL Reconnect();
private:
	RTMP*		m_pRTMP;
};

