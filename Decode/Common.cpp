#include "stdafx.h"
#include "Common.h"

namespace Decode
{
	INT ToINT32(BYTE val[4])
	{
		return (INT)(((val[0] & 0xFF) << 24) |
			((val[1] & 0xFF) << 16) |
			((val[2] & 0xFF) << 8) |
			((val[3] & 0xFF)));
	}
	INT ToINT24(BYTE val[3])
	{
		return (INT)(((val[0] & 0xFF) << 16) |
			((val[1] & 0xFF) << 8) |
			((val[2] & 0xFF)));
	}
	INT ToINT16(BYTE val[2])
	{
		return	(INT)(val[0] << 8) | (val[1]);
	}
	INT ToINT8(BYTE val[1])
	{
		return	(INT)(val[0] << 8);
	}
	BOOL FindStartCode(BYTE* bits, INT size)
	{
		for (INT i = 0;i < size - 1;i++)
		{
			if (bits[i] != 0)
				return FALSE;
		}
		if (bits[size - 1] != 1)
			return FALSE;
		return TRUE;
	}
	static INT AACSampleRates[16] =
	{
		96000, 88200, 64000, 48000, 44100, 32000,
		24000, 22050, 16000, 12000, 11025, 8000, 7350,
		0, 0, 0
	};
	//////////////////////////////////////////////////////////////////////////
	BOOL TS_PACKET_PROGRAM::operator == (const TS_PACKET_PROGRAM& other)
	{
		return ProgramPID == other.ProgramPID;
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL TS_PACKET_SERVICE_DESCRIPTOR::Parser(const BYTE* bits, LONG size)
	{
		INT index = 0;
		ServiceType = bits[index++];
		INT size1 = bits[index++];
		if (size1 > size)
			return FALSE;
		ProviderName.resize(size1 + 1);
		ProviderName[size1] = '\0';
		memcpy_s(&ProviderName[0], size1, &bits[index], size1);
		index += size1;
		INT size2 = bits[index++];
		if (size2 > size)
			return FALSE;
		ServiceName.resize(size2 + 1);
		ServiceName[size2] = '\0';
		memcpy_s(&ServiceName[0], size2, &bits[index], size2);
		index += size2;
		return index <= size;
	}
	//////////////////////////////////////////////////////////////////////////
	H264VideoConfig::H264VideoConfig()
		: m_codec(UnknownVideoCodec),
		m_profile(VIDEO_CODEC_PROFILE_UNKNOWN),
		m_pixelFormat(PIXEL_FORMAT_UNKNOWN),
		m_frameRate(0)
	{
	}
	H264VideoConfig::H264VideoConfig(VideoCodec codec,
		VideoCodecProfile profile,
		VideoPixelFormat pixelFormat,
		const INT frameRate,
		const TinySize& codedSize,
		const TinyRectangle& visibleRect,
		const TinySize& naturalSize)
		: m_codec(codec),
		m_profile(profile),
		m_pixelFormat(pixelFormat),
		m_frameRate(frameRate),
		m_codedSize(codedSize),
		m_visibleRect(visibleRect),
		m_naturalSize(naturalSize)
	{

	}
	H264VideoConfig::H264VideoConfig(const H264VideoConfig& o)
	{
		m_codec = o.m_codec;
		m_pixelFormat = o.m_pixelFormat;
		m_profile = o.m_profile;
		m_frameRate = o.m_frameRate;
		m_codedSize = o.m_codedSize;
		m_visibleRect = o.m_visibleRect;
		m_naturalSize = o.m_naturalSize;
	}
	BOOL H264VideoConfig::operator == (const H264VideoConfig& o)
	{
		return ((m_codec == o.m_codec) &&
			(m_pixelFormat == o.m_pixelFormat) &&
			(m_profile == o.m_profile) &&
			(m_frameRate == o.m_frameRate) &&
			(m_codedSize == o.m_codedSize) &&
			(m_visibleRect == o.m_visibleRect) &&
			(m_naturalSize == o.m_naturalSize));
	}
	BOOL H264VideoConfig::operator != (const H264VideoConfig& o)
	{
		return ((m_codec != o.m_codec) ||
			(m_pixelFormat != o.m_pixelFormat) ||
			(m_profile != o.m_profile) ||
			(m_frameRate != o.m_frameRate) ||
			(m_codedSize != o.m_codedSize) ||
			(m_visibleRect != o.m_visibleRect) ||
			(m_naturalSize != o.m_naturalSize));
	}
	//////////////////////////////////////////////////////////////////////////
	AACAudioConfig::AACAudioConfig()
		:m_codec(CodecAAC),
		m_wChannels(0),
		m_dwSampleRate(0),
		m_wBitsPerSample(0)
	{
	}
	AACAudioConfig::AACAudioConfig(AudioCodec codec, WORD wChannels, DWORD dwSampleRate, WORD wBitsPerSample)
		: m_codec(codec),
		m_wChannels(wChannels),
		m_dwSampleRate(dwSampleRate),
		m_wBitsPerSample(wBitsPerSample)
	{
	}
	AACAudioConfig::AACAudioConfig(const AACAudioConfig& o)
		: m_codec(o.m_codec),
		m_wChannels(o.m_wChannels),
		m_dwSampleRate(o.m_dwSampleRate),
		m_wBitsPerSample(o.m_wBitsPerSample)
	{

	}
	BOOL AACAudioConfig::operator == (const AACAudioConfig& o)
	{
		return (m_codec == o.m_codec) &&
			(m_wChannels == o.m_wChannels) &&
			(m_dwSampleRate == o.m_dwSampleRate) &&
			(m_wBitsPerSample == o.m_wBitsPerSample);
	}
	BOOL AACAudioConfig::operator != (const AACAudioConfig& o)
	{
		return (m_codec != o.m_codec) ||
			(m_wChannels != o.m_wChannels) ||
			(m_dwSampleRate != o.m_dwSampleRate) ||
			(m_wBitsPerSample != o.m_wBitsPerSample);
	}

	DWORD AACAudioConfig::GetSampleRate() const
	{
		ASSERT(m_dwSampleRate >= 0 && m_dwSampleRate < 16);
		return AACSampleRates[m_dwSampleRate];
	}
}