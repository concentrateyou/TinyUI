#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	/// <summary>
	/// ÒôÆµ°ü 16×Ö½Ú¶ÔÆë
	/// </summary>
	class AudioPacket
	{
	public:
		AudioPacket(UINT32 size);
		~AudioPacket();
		CHAR*	data();
		UINT32	size() const;
	private:
		UINT32					m_size;
		TinyScopedPtr<CHAR>		m_bits;
	};
}
