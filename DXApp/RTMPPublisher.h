#pragma once
#include "DXFramework.h"
extern "C"
{
#include "rtmp.h"
#include "amf.h"
#include "log.h"
}
#pragma comment(lib,"librtmp.lib")

class RTMPPublisher
{
public:
	RTMPPublisher();
	~RTMPPublisher();
public:
	BOOL Connect(const TinyString& url);
	BOOL SendMetadata(INT cx, INT cy, INT fps, INT rate);
	BOOL SendSPSPPS(const vector<BYTE>& pps, const vector<BYTE>& sps);
	BOOL SendVideoRTMP(BYTE* data, INT size, DWORD timeoffset);
	BOOL SendAudioRTMP(BYTE* data, INT size, DWORD timeoffset);
	BOOL Reconnect();
private:
	RTMP*		m_pRTMP;
};

