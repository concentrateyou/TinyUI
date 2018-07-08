#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualComboBoxItem : public TinyVisual
		{
			friend class TinyVisualComboBoxHWND;
			friend class TinyVisualComboBox;
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualComboBoxItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBoxItem)
		protected:
			TinyVisualComboBoxItem();
			TinyVisualComboBoxItem(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualComboBoxItem();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnMouseEnter() OVERRIDE;
			HRESULT		OnMouseHover(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseLeave() OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		public:
			void		SetImage(TinyImage* highlight);
		private:
			DWORD			m_dwFlag;
			TinyImage*		m_images[1];
		};
	}
}



