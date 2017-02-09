#pragma once
#include "Utility.h"
#include "mpg123.h"
#include "Media/TinyWave.h"
#pragma comment(lib,"libmpg123.lib")

namespace Decode
{
#define MPG123_INBUFFER_SIZE  16384
#define MPG123_OUTBUFFER_SIZE 32768
	/// <summary>
	/// MP3½âÂë
	/// </summary>
	class MPG123Decode
	{
	public:
		MPG123Decode();
		virtual ~MPG123Decode();
	public:
		BOOL Open();
		BOOL Decode(BYTE* rawdata, LONG rawsize);
		BOOL Close();
		BOOL SetFormat(WAVEFORMATEX& sFMT);
		WAVEFORMATEX* GetFormat();
	public:
		virtual void OnDone(BYTE*, LONG, LPVOID);
		Event<void(BYTE*, LONG, LPVOID)> EVENT_DONE;
	private:
		BYTE				  m_raw[MPG123_OUTBUFFER_SIZE];
		TinyScopedArray<BYTE> m_waveFMT;
		mpg123_handle*		  m_handle;
	};
}



