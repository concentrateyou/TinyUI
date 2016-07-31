#include "../stdafx.h"
#include <comdef.h>
#include "TinyRichText.h"

namespace TinyUI
{
	TinyRichText::TinyRichText()
	{

	}
	TinyRichText::~TinyRichText()
	{

	}
	BOOL TinyRichText::Initialize()
	{
		m_texthost.Reset(new TinyTextHost(this));
		return m_texthost->Initialize();
	}
	BOOL TinyRichText::PreTranslateMessage(MSG* pMsg)
	{
		HRESULT hRes = m_texthost->m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, 0);
		return FALSE;
	}
}