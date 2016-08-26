#pragma once
#include "Render/TinyGDI.h"
#include <string>
using namespace std;
using namespace TinyUI;

namespace Media
{
	enum VideoPixelFormat
	{
		PIXEL_FORMAT_UNKNOWN,
		PIXEL_FORMAT_I420,
		PIXEL_FORMAT_YUY2,
		PIXEL_FORMAT_UYVY,
		PIXEL_FORMAT_YV12,
		PIXEL_FORMAT_RGB24,
		PIXEL_FORMAT_RGB32,
		PIXEL_FORMAT_MJPEG,
		PIXEL_FORMAT_MAX,
	};

	enum
	{
		MaxDimension = (1 << 15) - 1,  // 32767
		MaxCanvas = (1 << (14 * 2)),  // 16384 x 16384
		MaxVideoFrames = 4,
		MaxSampleRate = 384000,
		MinSampleRate = 3000,
		MaxChannels = 32,
		MaxBytesPerSample = 4,
		MaxBitsPerSample = MaxBytesPerSample * 8,
		MaxSamplesPerPacket = MaxSampleRate,
		MaxPacketSizeInBytes = MaxBytesPerSample * MaxChannels * MaxSamplesPerPacket,
		MaxFramesPerSecond = 1000,
		MinCertificateLength = 128,
		MaxCertificateLength = 16 * 1024,
		MaxSessionIdLength = 512,
		MinKeyIdLength = 1,
		MaxKeyIdLength = 512,
		MaxKeyIds = 128,
		MaxInitDataLength = 64 * 1024,
		MaxSessionResponseLength = 64 * 1024,
		MaxKeySystemLength = 256,
	};

	class  VideoCaptureParam
	{
	public:
		VideoCaptureParam();
		VideoCaptureParam(const TinySize& size, FLOAT rate, VideoPixelFormat vpf);
		BOOL IsValid() const;
		static VideoPixelFormat GetFormat(const GUID& guid);
		void SetFormat(VideoPixelFormat vpf);
		void SetSize(const TinySize& size);
		void SetSize(INT cx, INT cy);
		void SetRate(FLOAT rate);
		const TinySize& GetSize() const;
		FLOAT	GetRate() const;
		VideoPixelFormat GetFormat() const;
		string ToString() const;
		static string PixelFormatToString(VideoPixelFormat format);
	private:
		FLOAT			 m_rate;
		TinySize		 m_size;
		VideoPixelFormat m_vpf;
	};
}