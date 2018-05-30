#include "../stdafx.h"
#include "TinyMsgQueue.h"

namespace TinyFramework
{
	namespace IO
	{
		TinyMsgQueue::TinyMsgQueue()
		{

		}
		TinyMsgQueue::~TinyMsgQueue()
		{

		}
		BOOL TinyMsgQueue::SetCallback(Callback<void(UINT, WPARAM, LPARAM)>&& callback)
		{
			m_callback = std::move(callback);
			return m_runner.Submit(BindCallback(&TinyMsgQueue::OnMessagePump, this));
		}
		BOOL TinyMsgQueue::Close()
		{
			PostThreadMessage(m_runner.GetID(), WM_QUIT, NULL, NULL);
			return m_runner.Close(INFINITE);
		}
		BOOL TinyMsgQueue::PostMsg(MSG& msg)
		{
			return PostThreadMessage(m_runner.GetID(), msg.message, msg.wParam, msg.lParam);
		}
		void TinyMsgQueue::OnMessagePump()
		{
			MSG msg = { 0 };
			for (;;)
			{
				if (!GetMessage(&msg, NULL, 0, 0))
				{
					if (!m_callback.IsNull())
					{
						m_callback(msg.message, msg.wParam, msg.lParam);
					}
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					break;
				if (!m_callback.IsNull())
				{
					m_callback(msg.message, msg.wParam, msg.lParam);
				}
			}
		}
	}
}
