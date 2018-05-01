#pragma once
#include "../IO/TinyThread.h"

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 消息队列
		/// </summary>
		class TinyMsgQueue
		{
			DISALLOW_COPY_AND_ASSIGN(TinyMsgQueue)
		public:
			TinyMsgQueue();
			~TinyMsgQueue();
			BOOL SetCallback(Callback<void(UINT, WPARAM, LPARAM)>&& callback);
			BOOL PostMsg(MSG& msg);
			BOOL Close();
		private:
			void OnMessagePump();
		private:
			TinyThread								m_runner;
			Callback<void(UINT, WPARAM, LPARAM)>	m_callback;
		};
	};
}

