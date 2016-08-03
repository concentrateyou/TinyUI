#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualRichText.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualRichText::TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

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
			TinyMessageLoop* pLoop = TinyApplication::GetInstance()->GetMessageLoop();
			ASSERT(pLoop != NULL);
			pLoop->AddMessageFilter(&m_texthost);
			return S_OK;
		}
		HRESULT TinyVisualRichText::OnDestory()
		{
			TinyMessageLoop* pLoop = TinyApplication::GetInstance()->GetMessageLoop();
			ASSERT(pLoop != NULL);
			pLoop->RemoveMessageFilter(&m_texthost);
			return S_OK;
		}
	}
}
