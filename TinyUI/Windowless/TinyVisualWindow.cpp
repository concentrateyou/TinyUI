#include "../stdafx.h"
#include "TinyVisualWindow.h"

namespace TinyUI
{
	namespace Windowless
	{
		const CHAR* TinyVisualWindow::Tag = TEXT("window");

		TinyVisualWindow::TinyVisualWindow(TinyVisualTree* myT)
			:m_myT(myT)
		{

		}
		TinyVisualWindow::~TinyVisualWindow()
		{

		}
		LPCSTR TinyVisualWindow::RetrieveTag()
		{
			return Tag;
		}

		BOOL TinyVisualWindow::ParseVisual(TiXmlElement* ps)
		{
			TiXmlAttribute* pFA = ps->FirstAttribute();
			TiXmlAttribute* pLA = ps->LastAttribute();
			while (pFA != pLA)
			{
				if (!strcasecmp(pFA->Name(), TinyVisual::NAME))
					this->SetName(pFA->Value());
				if (!strcasecmp(pFA->Name(), TinyVisual::TEXT))
					this->SetText(pFA->Value());
				if (!strcasecmp(pFA->Name(), TinyVisual::TOOLTIP))
					this->SetToolTip(pFA->Value());
				if (!strcasecmp(pFA->Name(), TinyVisual::MAXSIZE))
				{

				}
				pFA = pFA->Next();
			}
			return TRUE;
		}
		HRESULT TinyVisualWindow::OnDraw(TinyDC& dc, TinyRectangle& drawRect)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnMouseMove(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnLButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnLButtonUp(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnRButtonDown(POINT pos)
		{
			return FALSE;
		}

		HRESULT TinyVisualWindow::OnRButtonUp(POINT pos)
		{
			return FALSE;
		}

	}
}
