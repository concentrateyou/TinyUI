#include "../stdafx.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"
#include "TinyVisualDocument.h"
#include "TinyVisualRichText.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}

		HRESULT TinyVisualRichText::OnLButtonDown(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			LRESULT lRes = FALSE;
			m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnLButtonUp(const TinyPoint& pos, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			LRESULT lRes = FALSE;
			m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			LRESULT lRes = FALSE;
			m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			LRESULT lRes = FALSE;
			m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
		{
			ASSERT(m_texthost.m_ts);
			const MSG* pMsg = m_document->GetVisualHWND()->GetCurrentMessage();
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			LRESULT lRes = FALSE;
			m_texthost.m_ts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes);
			return lRes;
		}

		HRESULT TinyVisualRichText::OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage)
		{
			ASSERT(m_texthost.m_ts);
			m_document->GetVisualHWND()->SetMsgHandled(TRUE);
			POINT pos;
			GetCursorPos(&pos);
			::ScreenToClient(hWND, &pos);
			HDC hDC = GetDC(hWND);
			if (hDC != NULL)
			{
				TinyRectangle clip = GetWindowRect();
				m_texthost.m_ts->OnTxSetCursor(DVASPECT_CONTENT, -1, NULL, NULL, hDC, NULL, &clip, pos.x, pos.y);
				ReleaseDC(hWND, hDC);
			}
			return TRUE;
		}

		TinyVisualRichText::~TinyVisualRichText()
		{

		}
		TinyString TinyVisualRichText::RetrieveTag() const
		{
			return TinyVisualTag::RICHTEXT;
		}

		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_texthost.m_ts);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetFont((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			::SetGraphicsMode(canvas, GM_COMPATIBLE);
			TinyRectangle clip = GetWindowRect();
			m_texthost.m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, canvas, NULL, reinterpret_cast<LPCRECTL>(&clip), NULL, NULL, NULL, 0, TXTVIEW_ACTIVE);
			return TRUE;
		}
		HRESULT TinyVisualRichText::OnCreate()
		{
			m_texthost.Initialize(this);
			return S_OK;
		}
		HRESULT TinyVisualRichText::OnDestory()
		{
			return S_OK;
		}
	}
}
