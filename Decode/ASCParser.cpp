#include "stdafx.h"
#include "ASCParser.h"
#include <limits>
#include <algorithm>
using namespace std;

namespace Decode
{
	static const INT ADTSFrequencyTable[] =
	{
		96000, 88200, 64000, 48000, 44100,
		32000, 24000, 22050, 16000, 12000,
		11025, 8000,  7350
	};
	static const INT ADTSFrequencyTableSize = arraysize(ADTSFrequencyTable);

	static const AACChannelLayout ADTSChannelLayoutTable[] =
	{
		CHANNEL_LAYOUT_NONE,
		CHANNEL_LAYOUT_MONO,
		CHANNEL_LAYOUT_STEREO,
		CHANNEL_LAYOUT_SURROUND,
		CHANNEL_LAYOUT_4_0,
		CHANNEL_LAYOUT_5_0_BACK,
		CHANNEL_LAYOUT_5_1_BACK,
		CHANNEL_LAYOUT_7_1
	};
	static const INT ADTSChannelLayoutTableSize = arraysize(ADTSChannelLayoutTable);

	ASCParser::ASCParser()
		:m_audioObjectType(0),
		m_samplingFrequency(0),
		m_samplingFrequencyIndex(0),
		m_channelConfig(0),
		m_extensionFrequency(0),
		m_extensionAudioObjectType(0),
		m_extensionFrequencyIndex(0xFF),
		m_channelLayout(CHANNEL_LAYOUT_UNSUPPORTED)
	{

	}
	ASCParser::~ASCParser()
	{

	}
	BOOL ASCParser::Parse(const BYTE* bits, LONG size)
	{
		BOOL bFlag = FALSE;
		TinyBitReader reader;
		if (!reader.Initialize(bits, size))
			return FALSE;
		if (!reader.ReadBits(5, &m_audioObjectType))
			return FALSE;
		if (!reader.ReadBits(4, &m_samplingFrequencyIndex))
			return FALSE;
		if (!reader.ReadBits(4, &m_channelConfig))
			return FALSE;
		if (m_audioObjectType == 5 || m_audioObjectType == 29)
		{
			bFlag = (m_audioObjectType == 29);
			m_extensionAudioObjectType = 5;
			if (!reader.ReadBits(4, &m_extensionFrequencyIndex))
				return FALSE;
			if (m_extensionFrequencyIndex == 0xF)
			{
				if (!reader.ReadBits(24, &m_extensionFrequencyIndex))
					return FALSE;
			}
			if (!reader.ReadBits(5, &m_audioObjectType))
				return FALSE;
		}
		if (m_audioObjectType < 1 || m_audioObjectType  > 4)
			return FALSE;
		switch (m_audioObjectType)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
		case 17:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			SkipGASpecificConfig(reader);
			break;
		}
		if (m_extensionAudioObjectType != 5 && reader.Available() >= 16)
		{
			USHORT	syncExtensionType = 0;
			BYTE	sbrPresentFlag = 0;
			BYTE	psPresentFlag = 0;
			if (reader.ReadBits(11, &syncExtensionType) && syncExtensionType == 0x2b7)
			{
				if (reader.ReadBits(5, &m_extensionAudioObjectType) && m_extensionAudioObjectType == 5)
				{
					if (!reader.ReadBits(1, &sbrPresentFlag))
						return FALSE;
					if (sbrPresentFlag)
					{
						if (!reader.ReadBits(4, &m_extensionFrequencyIndex))
							return FALSE;
						if (m_extensionFrequencyIndex == 0xf)
						{
							if (!reader.ReadBits(24, &m_extensionFrequency))
								return FALSE;
						}
						if (reader.Available() >= 12)
						{
							if (!reader.ReadBits(11, &syncExtensionType))
								return FALSE;
							if (syncExtensionType == 0x548)
							{
								if (!reader.ReadBits(1, &psPresentFlag))
									return FALSE;
								bFlag = psPresentFlag != 0;
							}
						}
					}
				}
			}
		}
		if (m_samplingFrequency == 0)
		{
			if (m_samplingFrequencyIndex >= ADTSFrequencyTableSize)
				return FALSE;
			m_samplingFrequency = ADTSFrequencyTable[m_samplingFrequencyIndex];
		}
		if (m_extensionFrequency == 0 && m_extensionFrequencyIndex != 0xFF)
		{
			if (m_extensionFrequencyIndex >= ADTSFrequencyTableSize)
				return FALSE;
			m_extensionFrequencyIndex = ADTSFrequencyTable[m_extensionFrequencyIndex];
		}
		if (bFlag && m_channelConfig == 1)
		{
			m_channelLayout = CHANNEL_LAYOUT_STEREO;
		}
		else
		{
			if (m_channelConfig >= ADTSChannelLayoutTableSize)
				return FALSE;
			m_channelLayout = ADTSChannelLayoutTable[m_channelConfig];
		}
		return TRUE;
	}
	INT	ASCParser::GetFrequency() const
	{
		return m_samplingFrequency;
	}
	AACChannelLayout ASCParser::GetChannelLayout() const
	{
		return m_channelLayout;
	}
	BOOL ASCParser::SkipGASpecificConfig(TinyBitReader* reader) const
	{
		BYTE	extensionFlag = 0;
		BYTE	dependsOnCoreCoder = 0;
		USHORT	value = 0;
		if (!reader->ReadBits(1, &value))//0表示帧长为1024,1表示帧长为960
			return FALSE;
		if (!reader->ReadBits(1, &dependsOnCoreCoder))
			return FALSE;
		if (dependsOnCoreCoder == 1)
		{
			if (!reader->ReadBits(14, &value))
				return FALSE;
		}
		if (!reader->ReadBits(1, &extensionFlag))
			return FALSE;
		if (m_audioObjectType == 6 || m_audioObjectType == 20)
		{
			if (!reader->ReadBits(3, &value))
				return FALSE;
		}
		if (extensionFlag)
		{
			if (m_audioObjectType == 22)
			{
				if (!reader->ReadBits(5, &value))//numOfSubFrame
					return FALSE;
				if (!reader->ReadBits(11, &value))//layer_length
					return FALSE;
			}

			if (m_audioObjectType == 17 || m_audioObjectType == 19 || m_audioObjectType == 20 || m_audioObjectType == 23)
			{
				if (!reader->ReadBits(3, &value))//resilience flags
					return FALSE;
			}
			if (!reader->ReadBits(1, &value)) // extensionFlag3
				return FALSE;
		}
		return TRUE;
	}
}

