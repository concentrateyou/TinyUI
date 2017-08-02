#pragma once
#include "Common.h"
#include "mpg123.h"
#pragma comment(lib,"libmpg123.lib")

namespace Decode
{
#define MPG123_INBUFFER_SIZE  16384
#define MPG123_OUTBUFFER_SIZE 32768
	/// <summary>
	/// MP3½âÂë
	/// </summary>
	class MP3Decode
	{
	public:
		MP3Decode();
		virtual ~MP3Decode();
	public:
		BOOL Initialize(Callback<void(BYTE*, LONG, LPVOID)>&& callback);
		BOOL Open();
		BOOL Decode(BYTE* rawdata, LONG rawsize);
		BOOL Close();
		BOOL SetFormat(WAVEFORMATEX& sFMT);
		WAVEFORMATEX* GetFormat();
	private:
		BYTE				  m_raw[MPG123_OUTBUFFER_SIZE];
		TinyScopedArray<BYTE> m_waveFMT;
		mpg123_handle*		  m_handle;
		Callback<void(BYTE*, LONG, LPVOID)> m_callback;
	};
}



