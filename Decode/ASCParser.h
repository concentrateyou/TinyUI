#pragma once
#include "Common.h"
#include <vector>
using namespace std;
using namespace TinyFramework;

namespace Decode
{
	enum AACChannelLayout
	{
		CHANNEL_LAYOUT_NONE = 0,
		CHANNEL_LAYOUT_UNSUPPORTED = 1,
		CHANNEL_LAYOUT_MONO = 2,
		CHANNEL_LAYOUT_STEREO = 3,
		CHANNEL_LAYOUT_2_1 = 4,
		CHANNEL_LAYOUT_SURROUND = 5,
		CHANNEL_LAYOUT_4_0 = 6,
		CHANNEL_LAYOUT_2_2 = 7,
		CHANNEL_LAYOUT_QUAD = 8,
		CHANNEL_LAYOUT_5_0 = 9,
		CHANNEL_LAYOUT_5_1 = 10,
		CHANNEL_LAYOUT_5_0_BACK = 11,
		CHANNEL_LAYOUT_5_1_BACK = 12,
		CHANNEL_LAYOUT_7_0 = 13,
		CHANNEL_LAYOUT_7_1 = 14,
		CHANNEL_LAYOUT_7_1_WIDE = 15,
		CHANNEL_LAYOUT_STEREO_DOWNMIX = 16,
		CHANNEL_LAYOUT_3_1 = 18,
		CHANNEL_LAYOUT_4_1 = 19,
		CHANNEL_LAYOUT_6_0 = 20,
		CHANNEL_LAYOUT_6_0_FRONT = 21,
		CHANNEL_LAYOUT_HEXAGONAL = 22,
		CHANNEL_LAYOUT_6_1 = 23,
		CHANNEL_LAYOUT_6_1_BACK = 24,
		CHANNEL_LAYOUT_6_1_FRONT = 25,
		CHANNEL_LAYOUT_7_0_FRONT = 26,
		CHANNEL_LAYOUT_7_1_WIDE_BACK = 27,
		CHANNEL_LAYOUT_OCTAGONAL = 28,
		CHANNEL_LAYOUT_DISCRETE = 29,
		CHANNEL_LAYOUT_STEREO_AND_KEYBOARD_MIC = 30,
		CHANNEL_LAYOUT_4_1_QUAD_SIDE = 31,
		CHANNEL_LAYOUT_MAX = CHANNEL_LAYOUT_4_1_QUAD_SIDE
	};
	/// <summary>
	/// AudioSpecificConfig(AAC sequence header)½âÎö
	/// </summary>
	class ASCParser
	{
		DISALLOW_COPY_AND_ASSIGN(ASCParser)
	public:
		ASCParser();
		virtual ~ASCParser();
		BOOL				Parse(const BYTE* bits, LONG size);
		INT					GetFrequency() const;
		AACChannelLayout	GetChannelLayout() const;
	private:
		BOOL	SkipGASpecificConfig(TinyBitReader* reader) const;
	private:
		BYTE				m_audioObjectType;
		BYTE				m_samplingFrequencyIndex;
		BYTE				m_channelConfig;
		BYTE				m_extensionAudioObjectType;
		BYTE				m_extensionFrequencyIndex;
		INT					m_samplingFrequency;
		INT					m_extensionFrequency;
		AACChannelLayout	m_channelLayout;
	};
}
