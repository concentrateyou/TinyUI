#pragma once
#include "MShowCommon.h"
#include "FLVWriter.h"
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

namespace MShow
{
	class MRTMPClient
	{
		DISALLOW_COPY_AND_ASSIGN(MRTMPClient)
	public:
		MRTMPClient();
		~MRTMPClient();
	public:
		BOOL	Connect(const TinyString& szURL);
		BOOL	Disconnect();
		BOOL	Reconnect();
		BOOL	IsConnected();
		BOOL	IsTimedout();
		BOOL	SendMetadata(INT cx, INT cy, INT frameRate, INT videoRate, const WAVEFORMATEX& wfx, INT audioRate);
		BOOL	SendSPP(const vector<BYTE>& pps, const vector<BYTE>& sps);
		BOOL	SendAAC(BYTE* bits, LONG size);
		BOOL	SendVideo(DWORD dwFrameType, BYTE* bits, LONG size, DWORD timestamp);
		BOOL	SendAudio(BYTE* bits, LONG size, DWORD timestamp);
		DWORD	GetTime();
	private:
		RTMP*	m_pRTMP;
		Decode::FLVWriter	m_writer;
	};
}


