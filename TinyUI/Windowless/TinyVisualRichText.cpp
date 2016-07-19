#include "../stdafx.h"
#include "TinyVisualRichText.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document),
			m_bAllowBeep(FALSE),
			m_bAllowDrag(FALSE),
			m_bRichText(FALSE),
			m_dwStyle(0)
		{

		}
		TinyVisualRichText::~TinyVisualRichText()
		{

		}
		TinyString TinyVisualRichText::RetrieveTag() const
		{
			return TinyVisualTag::RICHTEXT;
		}

		BOOL TinyVisualRichText::PreTranslateMessage(MSG* pMsg)
		{
			ASSERT(m_texthost && m_texthost->m_pts);
			LRESULT lRes = 0;
			m_texthost->m_pts->TxSendMessage(pMsg->message, pMsg->wParam, pMsg->lParam, &lRes);
			return FALSE;
		}

		LRESULT TinyVisualRichText::OnCreate()
		{
			m_texthost.Reset(new TinyTextHost(this));
			TinyApplication::GetInstance()->GetMessageLoop()->AddMessageFilter(this);
			SetMultiline(TRUE);
			SetRichText(TRUE);
			TinyRectangle rectangle = m_document->GetWindowRect(this);
			m_texthost->SetRectangle(rectangle);
			m_texthost->SetCharFormat((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			m_texthost->SetParaFormat();
			m_texthost->m_pts->TxSetText(L"²âÊÔ11111");
			return FALSE;
		}
		LRESULT TinyVisualRichText::OnDestory()
		{
			TinyApplication::GetInstance()->GetMessageLoop()->RemoveMessageFilter(this);
			return FALSE;
		}
		BOOL TinyVisualRichText::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_texthost && m_texthost->m_pts);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			canvas.SetBrush((HBRUSH)GetStockObject(NULL_BRUSH));
			TinyRectangle s = m_document->GetWindowRect(this);
			return m_texthost->m_pts->TxDraw(
				DVASPECT_CONTENT,
				0,
				NULL,
				NULL,
				canvas.Handle(),
				NULL,
				(RECTL*)&s,
				NULL,
				(RECT*)&rcPaint,
				NULL,
				NULL,
				TXTVIEW_ACTIVE) == S_OK;
		}
		BOOL TinyVisualRichText::SetReadonly(BOOL fReadOnly)
		{
			if (fReadOnly)
			{
				m_dwStyle |= ES_READONLY;
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_READONLY, TXTBIT_READONLY) == S_OK;
			}
			else
			{
				m_dwStyle &= ~(DWORD)ES_READONLY;
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_READONLY, FALSE) == S_OK;
			}
		}
		BOOL TinyVisualRichText::SetPassword(BOOL fPassword)
		{
			if (fPassword)
			{
				m_dwStyle |= ES_PASSWORD;
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, TXTBIT_USEPASSWORD) == S_OK;
			}
			else
			{
				m_dwStyle &= ~(DWORD)ES_PASSWORD;
				return  m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_USEPASSWORD, FALSE) == S_OK;
			}
		}
		BOOL TinyVisualRichText::SetMultiline(BOOL fMultiline)
		{
			if (fMultiline)
			{
				m_dwStyle |= ES_MULTILINE;
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_MULTILINE, TXTBIT_MULTILINE) == S_OK;
			}
			else
			{
				m_dwStyle &= ~(DWORD)ES_MULTILINE;
				return  m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_MULTILINE, FALSE) == S_OK;
			}
		}
		BOOL TinyVisualRichText::SetRichText(BOOL bRichText)
		{
			m_bRichText = bRichText;
			if (bRichText)
			{
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, TXTBIT_RICHTEXT) == S_OK;
			}
			else
			{
				return  m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, FALSE) == S_OK;
			}
		}
		BOOL TinyVisualRichText::AllowBeep(BOOL bAllowBeep)
		{
			m_bAllowBeep = bAllowBeep;
			if (bAllowBeep)
			{
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, TXTBIT_ALLOWBEEP) == S_OK;
			}
			else
			{
				return  m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_ALLOWBEEP, FALSE) == S_OK;
			}
		}
		BOOL TinyVisualRichText::AllowDrag(BOOL bAllowDrag)
		{
			m_bAllowDrag = bAllowDrag;
			if (!bAllowDrag)
			{
				return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_DISABLEDRAG, TXTBIT_DISABLEDRAG) == S_OK;
			}
			else
			{
				return  m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_DISABLEDRAG, FALSE) == S_OK;
			}
		}
		BOOL TinyVisualRichText::SetLimit(DWORD dwLimit)
		{
			m_texthost->m_dwLimit = dwLimit;
			return m_texthost->m_pts->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE) == S_OK;
		}
	}
}
