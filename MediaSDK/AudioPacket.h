#pragma once
#include "MediaSDK.h"
#include "AudioParameters.h"

namespace MediaSDK
{
	/// <summary>
	/// ÒôÆµ°ü 16×Ö½Ú¶ÔÆë
	/// </summary>
	class AudioPacket
	{
	public:
		AudioPacket(const AudioParameters& params);
		~AudioPacket();
		void	Reset(const UINT32& size);
		CHAR*	data();
		WORD	count() const;
		UINT32	size() const;
		WORD	Channels() const;
		WORD	BitsPerSample() const;
	private:
		WORD					m_wChannels;
		WORD					m_wBitsPerSample;
		WORD					m_wCount;
		UINT32					m_size;
		TinyScopedPtr<CHAR>		m_bits;
	};
}
