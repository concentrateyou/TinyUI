#pragma once
#include "TinyVisualLabel.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualMenu;

		class TinyVisualMenuItem : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualMenuItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualMenuItem)
		protected:
			TinyVisualMenuItem();
			TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualMenuItem();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnMouseEnter() OVERRIDE;
			HRESULT		OnMouseLeave() OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
		public:
			void		SetSeparator(BOOL bSeparator);
			BOOL		IsSeparator() const;
			void		SetChecked(BOOL bChecked);
			BOOL		IsChecked() const;
			void		SetImageList(TinyImage* icon,
				TinyImage* highlight,
				TinyImage* check,
				TinyImage* arrow);
		private:
			DWORD			m_dwFlag;
			TinyImage*		m_images[4];
			TinyVisualMenu* m_child;
		};
	}
}



