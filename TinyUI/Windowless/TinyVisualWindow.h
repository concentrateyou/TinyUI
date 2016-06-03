#pragma once
#include "TinyVisual.h"

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
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT OnDraw(TinyDC& dc, TinyRectangle& drawRect) OVERRIDE;
			HRESULT OnMouseMove(POINT pos) OVERRIDE;
			HRESULT OnLButtonDown(POINT pos) OVERRIDE;
			HRESULT OnLButtonUp(POINT pos) OVERRIDE;
			HRESULT OnRButtonDown(POINT pos) OVERRIDE;
			HRESULT OnRButtonUp(POINT pos) OVERRIDE;
		public:
			void OnSizeChange(const TinySize&oldSize, const TinySize&newSize) OVERRIDE;
		private:
			TinyVisualTree*		m_myT;
			TinySize			m_size;
		};
	}
}



