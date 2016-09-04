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
	void SetSPS(BYTE* sps, DWORD dwSPS);
	void SetPPS(BYTE* pps, DWORD dwPPS);
	BOOL SendSPSPPS();
	BOOL SendVideo(BYTE* pBits, INT size);
	BOOL Reconnect();

private:
	RTMP*		m_pRTMP;
	DWORD		m_dwStart;
	DWORD		m_dwSPS;
	DWORD		m_dwPPS;
	BYTE		m_sps[10000];
	BYTE		m_pps[10000];
};

