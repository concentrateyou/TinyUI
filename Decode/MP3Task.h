#pragma once
#include "MPG123Decode.h"
#include "Media/TinyMP3File.h"
#include "Common/TinyCallback.h"
#include "IO/TinyThread.h"
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;

namespace Decode
{
	/// <summary>
	/// MP3解码成PCM并按AAC编码4096输出
	/// </summary>
	/*class MP3Task : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MP3Task)
	public:
		MP3Task();
		virtual ~MP3Task();
		BOOL Submit(LPCSTR pzFile);
		BOOL Close(DWORD dwMS) OVERRIDE;
		WAVEFORMATEX GetFormat() const;
	public:
		Event<void(BYTE*, LONG)>	EVENT_AUDIO;
	private:
		void OnMessagePump();
		void OnMP3Audio(BYTE*, LONG, LPVOID);
	private:
		BOOL					m_bBreak;
		MPG123Decode			m_decode;
		TinyMP3File				m_reader;
		WAVEFORMATEX			m_waveFMT;
		TinyBufferArray<BYTE>	m_residial;
	};*/
}



