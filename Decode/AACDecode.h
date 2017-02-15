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
		BOOL Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		BOOL Open(WORD wBitsPerSample, WORD wSampleRate);
		BOOL Open(BYTE* adts, LONG size, WORD wBitsPerSample);
		BOOL Decode(BYTE* bits, LONG size);
		BOOL Close();
		WAVEFORMATEX GetFormat() const;
	private:
		DWORD					m_dwINC;
		WAVEFORMATEX			m_sMFT;
		NeAACDecHandle			m_handle;
		NeAACDecFrameInfo		m_frame;
		Callback<void(BYTE*, LONG, LPVOID)> m_callback;
	};
}

