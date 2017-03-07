#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Render/TinyGDI.h"
extern "C"
{
#include "faac.h"
#include "faaccfg.h"
}
#pragma comment(lib,"libfaac.lib")
using namespace TinyUI;

namespace Encode
{
#define  AAC_TIMEBASE 1024
#define  AAC_TIMEDEN  1000
	class AACEncode
	{
	public:
		AACEncode();
		~AACEncode();
	public:
		BOOL	Open(const WAVEFORMATEX& waveFMT, INT audioRate, BOOL bAllowF = FALSE);
		BOOL	Encode(BYTE* bits, LONG size, BYTE*& bo, LONG& so);
		void	Close();
		BOOL	GetSpecificInfo(vector<BYTE>& info);
	private:
		faacEncHandle			m_aac;
		faacEncConfigurationPtr	m_config;
		BOOL					m_bAllowFloat;
		DWORD					m_inputSamples;
		DWORD					m_maxOutputBytes;
		DWORD					m_dwINC;
		DWORD					m_dwPTS;
		TinyScopedArray<BYTE>	m_bits;
		WAVEFORMATEX			m_waveFMT;
	};
}
