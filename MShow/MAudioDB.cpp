#include "stdafx.h"
#include "MAudioDB.h"
#include "Windowless/TinyVisualDocument.h"
#include "Windowless/TinyVisualManage.h"

namespace MShow
{
	IMPLEMENT_DYNCREATE(MAudioDB, TinyVisual);

	MAudioDB::MAudioDB()
		:m_db(0)
	{
	}

	MAudioDB::MAudioDB(TinyVisual* spvisParent, TinyVisualDocument* vtree)
		: TinyVisual(spvisParent, vtree),
		m_db(0)
	{

	}

	MAudioDB::~MAudioDB()
	{
	}
	TinyString MAudioDB::RetrieveTag() const
	{
		return TEXT("audiodb");
	}
	void MAudioDB::SetDB(INT db)
	{
		m_db = db;
	}
	BOOL MAudioDB::OnDraw(HDC hDC, const RECT& rcPaint)
	{
		if (!m_document)
			return FALSE;
		TinyClipCanvas canvas(hDC, this, rcPaint);
		TinyRectangle clip = m_document->GetWindowRect(this);
		if (m_backgroundImage != NULL && !m_backgroundImage->IsEmpty())
		{
			TinyRectangle srcRect = m_backgroundImage->GetRectangle();
			TinyRectangle srcCenter = GetBackgroundCenter();
			if (srcCenter.IsRectEmpty())
			{
				canvas.DrawImage(*m_backgroundImage, clip, srcRect);
			}
			else
			{
				canvas.DrawImage(*m_backgroundImage, clip, srcRect, srcCenter);
			}
		}
		//»æÖÆ¾ØÐÎ
		TinyBrush brush1;
		brush1.CreateBrush(RGB(193, 220, 226));
		canvas.SetBrush(brush1);
		for (INT i = 0;i < 10;i++)
		{
			TinyRectangle s;
			s.left = clip.left + i * 4;
			s.top = clip.top;
			s.right = s.left + 3;
			s.bottom = s.top + 14;
			canvas.FillRectangle(s);
		}
		if (m_db > 0)
		{
			INT count = m_db / 10 + 1;
			for (INT i = 0;i < count;i++)
			{
				TinyBrush brush2;
				TinyRectangle s;
				s.left = clip.left + i * 4;
				s.top = clip.top;
				s.right = s.left + 3;
				s.bottom = s.top + 14;
				brush2.CreateBrush(RGB(50, 154, 50));
				canvas.SetBrush(brush2);
				canvas.FillRectangle(s);
			}
		}
		return TinyVisual::OnDraw(hDC, rcPaint);
	}
}