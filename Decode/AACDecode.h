#pragma once
#include "Utility.h"
#include "faad.h"
using namespace TinyUI;

namespace Decode
{
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4
	typedef struct tagMediaTag
	{
		DWORD	dwType;
		DWORD	dwFlag;
		DWORD	dwINC;
		DWORD	dwTime;
		DWORD	dwPTS;
		DWORD	dwDTS;
	}MediaTag;

	class AACDecode
	{
	public:
		AACDecode();
		virtual ~AACDecode();
		BOOL Open(WORD wBitsPerSample = 16, WORD wSampleRate = 44100);
		BOOL Decode(BYTE* bits, LONG size, DWORD& dwINC);
		BOOL Close();
	public:
		virtual void OnDone(BYTE*, LONG, const MediaTag&);
		Event<void(BYTE*, LONG, const MediaTag&)> EVENT_DONE;
	private:
		DWORD					m_dwINC;
		NeAACDecHandle			m_handle;
		NeAACDecConfiguration	m_config;
		NeAACDecFrameInfo		m_frame;
	};
}

