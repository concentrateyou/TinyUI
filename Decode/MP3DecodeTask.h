#pragma once
#include "MP3Decode.h"
#include "Media/TinyMP3File.h"
#include "Common/TinyCallback.h"
#include "IO/TinyTaskBase.h"
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;

namespace Decode
{
	/// <summary>
	/// MP3Ω‚¬ÎPCM
	/// </summary>
	class MP3DecodeTask : public TinyTaskBase
	{
		DISALLOW_COPY_AND_ASSIGN(MP3DecodeTask)
	public:
		MP3DecodeTask(TinyUI::Callback<void(BYTE*, LONG)>&& callback);
		virtual ~MP3DecodeTask();
		BOOL Submit(LPCSTR pzFile);
		BOOL Close(DWORD dwMS) OVERRIDE;
		WAVEFORMATEX GetFormat() const;
	private:
		void OnMessagePump();
		void OnMP3Audio(BYTE*, LONG, LPVOID);
	private:
		BOOL					m_bBreak;
		MP3Decode				m_decode;
		TinyMP3File				m_mp3File;
		WAVEFORMATEX			m_waveFMT;
		TinyUI::Callback<void(BYTE*, LONG)>	m_callback;
	};
}



