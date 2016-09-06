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
	BOOL SendMetadata(INT cx, INT cy, INT rate);
	BOOL SendSPSPPS(const vector<BYTE>& pps, const vector<BYTE>& sps);
	BOOL SendVideoRTMP(BYTE* data, INT size);
	BOOL SendAudioRTMP(BYTE* data, INT size);
	BOOL Reconnect();
private:
	RTMP*		m_pRTMP;
	DWORD		m_dwStart;
};

