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
	class AACDecode
	{
	public:
		AACDecode();
		virtual ~AACDecode();
		BOOL Open(WORD wBitsPerSample, WORD wSampleRate);
		BOOL Open(BYTE* adts, LONG size, WORD wBitsPerSample);
		BOOL Decode(BYTE* bits, LONG size);
		BOOL Close();
		WAVEFORMATEX GetFormat() const;
	public:
		virtual void OnDone(BYTE*, LONG, LPVOID);
		Event<void(BYTE*, LONG, LPVOID)> EVENT_DONE;
	private:
		WAVEFORMATEX			m_sMFT;
		NeAACDecHandle			m_handle;
		DWORD					m_dwINC;
		NeAACDecFrameInfo		m_frame;
	};
}

