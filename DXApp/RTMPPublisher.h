#pragma once
#include "DXFramework.h"
extern "C"
{
#include "rtmp.h"
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
	BOOL SendSPSPPS(const vector<BYTE>& pps, const vector<BYTE>& sps);
	BOOL SendVideoRTMP(BYTE* pBits, INT size);
	BOOL Reconnect();
private:
	RTMP*		m_pRTMP;
	DWORD		m_dwStart;
};

