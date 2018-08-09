#pragma once
#include "MediaSDK.h"

namespace MediaSDK
{
	/// <summary>
	/// ��Ƶ�� 16�ֽڶ���
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
		UINT32										m_count;//֡��
		vector<FLOAT*>								m_channels;
		TinyScopedPtr<FLOAT, AlignedFreeDeleter>	m_bits;
	};
}
