#pragma once
#include "TinyVisualElement.h"
#include "TinyVisualHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualTree;
		class TinyVisualWindow : public TinyVisual
		{
		public:
			TinyVisualWindow(TinyVisualTree* myT);
			virtual ~TinyVisualWindow();
		public:
			LPCSTR RetrieveTag() OVERRIDE;
			BOOL ParseVisual(TiXmlElement* ps) OVERRIDE;
			HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		public:
			static const CHAR* Tag;
		private:
			TinyVisualTree*		m_myT;
		};
	}
}



