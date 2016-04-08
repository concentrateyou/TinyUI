#include "../stdafx.h"
#include "TinyVisualWindow.h"
#include "../Common/TinyString.h"

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

		BOOL TinyVisualWindow::ParsePropertys(TiXmlElement* ps)
		{
			TiXmlAttribute* pFA = ps->FirstAttribute();
			TiXmlAttribute* pLA = ps->LastAttribute();
			while (pFA != pLA)
			{
				if (!strcasecmp(pFA->Name(), TinyVisualPoperty::SIZE))
				{
					TinyString val = pFA->Value();
					TinyArray<TinyString> sps;
					val.Split(',', sps);
					if (sps.GetSize() == 2)
					{
						this->SetSize(TinySize(atoi(sps[0].STR()), atoi(sps[1].STR())));
					}
				}
				pFA = pFA->Next();
			}
			return TinyVisual::ParsePropertys(ps);
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
		void TinyVisualWindow::SetSize(const TinySize& size)
		{
			if (m_size != size)
			{
				m_size = size;
			}
		}
		TinySize TinyVisualWindow::GetSize() const
		{
			return m_size;
		}
	}
}
