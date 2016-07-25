#include "../stdafx.h"
#include "TinyVisualRichText.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
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
			HWND hWND = m_document->GetVisualHWND()->Handle();
			TinyApplication::GetInstance()->GetMessageLoop()->AddMessageFilter(this);
			m_texthost.Reset(new TinyTextHost(this));
			HDC hDC = GetDC(hWND);
			HFONT hOldFont = (HFONT)SelectObject(hDC, GetStockObject(SYSTEM_FONT));
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SelectObject(hDC, hOldFont);
			xWidthSys = (INT)tm.tmAveCharWidth;
			yHeightSys = (INT)tm.tmHeight;
			xPerInch = GetDeviceCaps(hDC, LOGPIXELSX);
			yPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
			TinyRectangle rectangle = m_document->GetWindowRect(this);
			m_texthost->SetRectangle(rectangle);
			m_sizelExtent.cx = DXtoHimetricX(TO_CX(rectangle) - 2 * HOST_BORDER, xPerInch);
			m_sizelExtent.cy = DYtoHimetricY(TO_CY(rectangle) - 2 * HOST_BORDER, yPerInch);
			m_texthost->SetCharFormat((HFONT)GetStockObject(DEFAULT_GUI_FONT));
			m_texthost->SetParaFormat();
			m_texthost->SetRectangle(rectangle);
			m_texthost->SetText("²âÊÔ11111");
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
	}
}
