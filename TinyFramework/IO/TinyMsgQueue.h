#pragma once
#include "../IO/TinyThread.h"

namespace TinyFramework
{
	namespace IO
	{
		/// <summary>
		/// ��Ϣ����
		/// </summary>
		class TinyMsgQueue
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMsgQueue)
		public:
			TinyMsgQueue();
			~TinyMsgQueue();
			void SetCallback(Callback<void(UINT, WPARAM, LPARAM)>&& callback);
			BOOL PostMsg(MSG& msg);
			BOOL Open();
			BOOL Close();
		private:
			void OnMessagePump();
		private:
			TinyThread								m_runner;
			Callback<void(UINT, WPARAM, LPARAM)>	m_callback;
		};
	};
}

