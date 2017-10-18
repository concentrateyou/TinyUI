#pragma once
#include "Common/TinyCommon.h"
#include "Common/TinyCallback.h"
#include "Common/TinyEvent.h"
#include "Render/TinyGDI.h"
#include "Media/TinyMedia.h"
extern "C"
{
#include "faac.h"
#include "faaccfg.h"
}
#pragma comment(lib,"libfaac.lib")
using namespace TinyUI;
using namespace TinyUI::Media;

namespace Encode
{
#define  AAC_TIMEBASE 1024.0
#define  AAC_TIMEDEN  1000
	class AACEncode
	{
		DISALLOW_COPY_AND_ASSIGN(AACEncode)
	public:
		AACEncode();
		virtual ~AACEncode();
	public:
		BOOL	Open(const WAVEFORMATEX& waveFMT, INT audioRate = 128);
		BOOL	Encode(BYTE* bi, LONG si, BYTE*& bo, LONG& so, MediaTag& tag);
		void	Close();
		BOOL	GetSpecificInfo(vector<BYTE>& info);
		DWORD	GetOutputBytes() const;
	private:
		faacEncHandle			m_aac;
		faacEncConfigurationPtr	m_config;
		DWORD					m_inputSamples;
		DWORD					m_maxOutputBytes;
		DWORD					m_dwINC;
		DOUBLE					m_timestamp;
		FLOAT					m_fPTS;
		WAVEFORMATEX			m_waveFMT;
		TinyScopedArray<BYTE>	m_bits;
	};
}
