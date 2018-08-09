#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	/// <summary>
	/// 音频包 16字节对齐
	/// </summary>
	class AudioPacket
	{
	public:
		AudioPacket(UINT32 channels, UINT32 count);
		~AudioPacket();
	public:
		void Scale(FLOAT volume);
		void Copy(UINT32 channels, FLOAT* data);
	private:
		UINT32										m_count;//帧数
		vector<FLOAT*>								m_channels;
		TinyScopedPtr<FLOAT, AlignedFreeDeleter>	m_bits;
	};
}
