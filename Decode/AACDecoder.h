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
	class AACDecoder
	{
		DISALLOW_COPY_AND_ASSIGN(AACDecoder)
	public:
		AACDecoder();
		virtual ~AACDecoder();
		BOOL Open(WORD wBitsPerSample, WORD wSampleRate);
		BOOL Open(BYTE* adts, LONG size, WORD wBitsPerSample);
		BOOL Decode(SampleTag& tag, BYTE*& bo, LONG& so);
		BOOL Close();
		NeAACDecHandle	GetHandle();
		WAVEFORMATEX*	GetFormat();
	private:
		WAVEFORMATEX			m_sMFT;
		NeAACDecHandle			m_handle;
		NeAACDecFrameInfo		m_frame;
	};
}

