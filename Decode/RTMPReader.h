#pragma once
#include "Common.h"
#include "rtmp.h"

namespace Decode
{
	/// <summary>
	/// FLV大端数据
	/// 目前音频支持支AAC,MP3和PCM,视频H.264
	/// </summary>
	class RTMPReader : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(RTMPReader)
	public:
		RTMPReader();
		~RTMPReader();
		BOOL Open(LPCSTR pzURL);
		BOOL Close(DWORD dwMS) OVERRIDE;
	public:
		Event<void(BYTE*, LONG, FLV_PACKET*)> EVENT_AUDIO;
		Event<void(BYTE*, LONG, FLV_PACKET*)> EVENT_VIDEO;
		Event<void(FLV_SCRIPTDATA*)> EVENT_SCRIPT;
	private:
		void OnMessagePump();
		BOOL ParseVideo(BYTE* data, INT size);
		BOOL ParseAudio(BYTE* data, INT size);
		BOOL ParseScript(BYTE* data, INT size);
		BOOL ParseAAC(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseMP3(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParsePCM(FLV_TAG_AUDIO* audio, BYTE* data, INT size);
		BOOL ParseH264(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseMPEG4(FLV_TAG_VIDEO* video, BYTE* data, INT size);
		BOOL ParseNALU(FLV_TAG_VIDEO* video, INT* cts, BYTE* data, INT size);
	private:
		BYTE		m_minusOne;
		RTMP		m_sRTMP;
		TinyEvent	m_close;
		LONGLONG	m_timestamp;
	};

	class PacketQueue
	{
	public:
		PacketQueue(TinyLock& lock);
		~PacketQueue();
		void Push(Media::SampleTag& tag);
		Media::SampleTag Pop();
		INT GetSize() const;
		BOOL IsEmpty() const;
		INT GetCount() const;
	private:
		INT						m_size;
		TinyLock&				m_lock;
		TinyLinkList<Media::SampleTag>	m_list;
	};
}


