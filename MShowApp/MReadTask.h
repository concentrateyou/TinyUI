#pragma once
#include "MShowCommon.h"
#include "FLVReader.h"
using namespace Decode;

namespace MShow
{
	class MReadTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MReadTask)
	public:
		MReadTask();
		~MReadTask();
		BOOL Initialize(LPCSTR pzURL);
		BOOL Close(DWORD dwMS) OVERRIDE;
	private:
		void OnMessagePump();
	private:
		BOOL			m_close;
		FLVReader		m_reader;
		FLV_SCRIPTDATA	m_script;
		PacketQueue					m_audioQueue;
		PacketQueue					m_videoQueue;
	};
}



