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
using namespace Decode;

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
		BOOL	SendScript(INT cx, INT cy, INT frameRate, INT videoRate, const WAVEFORMATEX& wfx, INT audioRate);
		BOOL	SendH264AVC(const vector<BYTE>& pps, const vector<BYTE>& sps);
		BOOL	SendAACASC(BYTE* bits, LONG size);
		BOOL	SendH264NALU(DWORD dwFrameType, BYTE* bits, LONG size, DWORD timestamp);
		BOOL	SendAACRaw(BYTE* bits, LONG size, DWORD timestamp);
		DWORD	GetTime();
	private:
		BOOL	BuildScript(const FLV_SCRIPTDATA& script, TinyBufferArray<BYTE>& buffer);
		BOOL	BuildH264AVC(const vector<BYTE>& pps, const vector<BYTE>& sps, TinyBufferArray<BYTE>& buffer);
		BOOL	BuildAACASC(BYTE* bits, LONG size, TinyBufferArray<BYTE>& buffer);
		BOOL	BuildH264NALU(DWORD dwFrameType, BYTE* bits, LONG size, DWORD timestamp, TinyBufferArray<BYTE>& buffer);
		BOOL	BuildAACRaw(BYTE* bits, LONG size, DWORD timestamp, TinyBufferArray<BYTE>& buffer);
		BOOL	BuildAudioTag(FLV_PACKET& packet, BYTE* bits, LONG size, TinyBufferArray<BYTE>& buffer);
		BOOL	BuildVideoTag(FLV_PACKET& packet, BYTE* bits, LONG size, TinyBufferArray<BYTE>& buffer);
	private:
		DWORD		m_dwPreviousSize;
		RTMP*		m_pRTMP;
		FLVWriter	m_writer;
	};
}


