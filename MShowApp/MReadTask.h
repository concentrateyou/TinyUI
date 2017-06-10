#pragma once
#include "MShowCommon.h"
#include "MPacketQueue.h"
#include "FLVReader.h"
using namespace Decode;

namespace MShow
{
	/// <summary>
	/// RTMP流读取线程
	/// </summary>
	class MReadTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MReadTask)
	public:
		MReadTask();
		virtual ~MReadTask();
		BOOL Initialize(LPCSTR pzURL);
		BOOL Submit();
		BOOL Close(DWORD dwMS) OVERRIDE;
	public:
		const FLV_SCRIPTDATA& GetScript() const;
		TinyLock& GetAudioLock();
		TinyLock& GetVideoLock();
		MPacketQueue& GetAudioQueue();
		MPacketQueue& GetVideoQueue();
		H264Decode* GetH264();
		AACDecode* GetAAC();
	private:
		void OnMessagePump();
	private:
		BOOL			m_bFI;
		LONGLONG		m_sample;
		TinyEvent		m_close;
		FLVReader		m_reader;
		FLV_SCRIPTDATA	m_script;
		TinyLock		m_locks[2];
		MPacketQueue	m_audioQueue;
		MPacketQueue	m_videoQueue;
		TinyScopedPtr<H264Decode>	m_h264;
		TinyScopedPtr<AACDecode>	m_aac;
	};
}



