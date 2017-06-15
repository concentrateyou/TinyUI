#pragma once
#include "Common.h"
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
		DISALLOW_COPY_AND_ASSIGN(AACDecode)
	public:
		AACDecode();
		virtual ~AACDecode();
		BOOL Open(WORD wBitsPerSample, WORD wSampleRate);
		BOOL Open(BYTE* adts, LONG size, WORD wBitsPerSample);
		BOOL Decode(BYTE* bi, LONG si, BYTE*& bo, LONG& so);
		BOOL Close();
		WAVEFORMATEX* GetFormat();
	private:
		DWORD					m_dwINC;
		WAVEFORMATEX			m_sMFT;
		NeAACDecHandle			m_handle;
		NeAACDecFrameInfo		m_frame;
	};
}

