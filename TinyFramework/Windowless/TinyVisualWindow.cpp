#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualWindow, TinyVisual);

		TinyVisualWindow::TinyVisualWindow()
		{

		}
		TinyVisualWindow::TinyVisualWindow(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}

		void TinyVisualWindow::OnSizeChange(const TinySize& oldsize, const TinySize& newsize)
		{
			ASSERT(m_document);
			TinyVisual* spvis = m_spvisChild;
			while (spvis != NULL && spvis->IsVisible())
			{
				TinySize size = spvis->GetSize();
				spvis->SetSize(TinySize(newsize.cx, size.cy));
				spvis->OnSizeChange(size, spvis->GetSize());
				spvis = m_document->GetVisual(spvis, CMD_NEXT);
			}
		}

		void TinyVisualWindow::SetPosition(const TinyPoint& pos)
		{
			TinyVisual::SetPosition(TinyPoint(0, 0));
		}

		void TinyVisualWindow::SetText(const TinyString& pzText)
		{
			ASSERT(m_document);
			::SetWindowText(m_document->GetVisualHWND().Handle(), pzText.CSTR());
			TinyVisual::SetText(pzText);
		}

		BOOL TinyVisualWindow::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::SHADOWIMAGE.STR()) == 0)
			{
				if (m_document != NULL)
				{
					TinyVisualShadow* shadow = m_document->GetVisualHWND().GetShadow();
					if (shadow != NULL)
					{
						shadow->SetShadow(TinyVisualResource::GetInstance()[value]);
					}
				}
				return TRUE;
			}
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::SHADOWBOX.STR()) == 0)
			{
				if (m_document != NULL)
				{
					TinyVisualShadow* shadow = m_document->GetVisualHWND().GetShadow();
					ASSERT(shadow);
					shadow->SetShadowBox(TinyVisualBuilder::GetRectangle(value));
				}
				return TRUE;
			}
			return TinyVisual::SetProperty(name, value);
		}

		TinyString TinyVisualWindow::RetrieveTag() const
		{
			return TinyVisualTagConst::WINDOW;
		}
		BOOL TinyVisualWindow::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (m_borderImage != NULL && !m_borderImage->IsEmpty())
			{
				TinyRectangle srcRect = m_borderImage->GetRectangle();
				TinyRectangle srcCenter = GetBorderCenter();
				if (srcCenter.IsRectEmpty())
				{
					canvas.DrawImage(*m_borderImage, clip, srcRect);
				}
				else
				{
					canvas.DrawImage(*m_borderImage, clip, srcRect, srcCenter);
				}
			}
			if (!m_borderColor.IsTransparent() && m_borderThickness != -1)
			{
				TinyPen pen;
				pen.CreatePen(m_borderStyle, m_borderThickness, m_borderColor);
				HPEN hOldPen = canvas.SetPen(pen);
				canvas.DrawRectangle(clip);
				canvas.SetPen(hOldPen);
			}
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
			if (!m_backgroundColor.IsTransparent())
			{
				TinyBrush brush;
				brush.CreateBrush(m_backgroundColor);
				HBRUSH hOldBrush = canvas.SetBrush(brush);
				canvas.FillRectangle(clip);
				canvas.SetBrush(hOldBrush);
			}
			return TRUE;
		}
	}
}
