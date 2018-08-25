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
		void	SetCapacity(const UINT32& capacity);
		void	SetSize(const UINT32& size);
		CHAR*	data();
		UINT32  size() const;
		UINT32	capacity() const;
		WORD	Frames() const;
		WORD	Channels() const;
		WORD	BitsPerSample() const;
	private:
		WORD					m_wChannels;
		WORD					m_wBitsPerSample;
		WORD					m_wFrames;
		UINT32					m_size;
		UINT32					m_capacity;
		TinyScopedPtr<CHAR>		m_bits;
	};
}
