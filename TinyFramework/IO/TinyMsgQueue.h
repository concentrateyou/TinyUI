#pragma once
#include "../IO/TinyWorker.h"

namespace TinyFramework
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
			void SetCallback(Callback<void(UINT, WPARAM, LPARAM)>&& callback);
			BOOL PostMsg(MSG& msg);
			BOOL Open();
			BOOL Close();
		private:
			void OnMessagePump();
		private:
			TinyWorker								m_runner;
			Callback<void(UINT, WPARAM, LPARAM)>	m_callback;
		};
	};
}

