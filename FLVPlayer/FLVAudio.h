#pragma once
#include "Media/TinySoundPlayer.h"
#include "FLVParser.h"
#include "FLVPlayer.h"
#include "Common/TinyTime.h"
#include "IO/TinyTaskBase.h"
using namespace Decode;
using namespace TinyUI;
using namespace TinyUI::IO;
using namespace TinyUI::Media;

namespace FLVPlayer
{

	class FLVAudio : public TinyTaskBase
	{
	public:
		FLVAudio(HWND hWND);
		virtual ~FLVAudio();
		BOOL	Submit();
		BOOL	Close(DWORD dwMs) OVERRIDE;
	private:
		void	OnMessagePump();
	private:
		HWND						m_hWND;
		BOOL						m_bInitialize;
		TinyEvent					m_close;
		TinyScopedPtr<AACDecode>	m_aac;
		TinyBufferArray<BYTE>		m_audios;
		TinySoundPlayer				m_player;
		TinyEvent					m_event[2];
		FLVReader					m_reader;
	};

}


