#pragma once
#include "MShowCommon.h"

namespace TSPlayer
{
	/// <summary>
	/// ���ʱ���,��Ƶͬ����Ƶ
	/// </summary>
	class MClock
	{
		DISALLOW_COPY_AND_ASSIGN(MClock)
	public:
		MClock();
		~MClock();
		void SetAudioPTS(LONGLONG audioPTS);
		LONGLONG GetAudioPTS();
		BOOL LockVideo(DWORD dwMS = INFINITE);
		void UnlockVideo();
	private:
		LONGLONG	m_audioPTS;
		TinyEvent	m_signal;
	};
}

