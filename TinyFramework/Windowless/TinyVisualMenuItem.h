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
			HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseLeave() OVERRIDE;
		public:
			void		SetImage(TinyImage* hl, TinyImage* checked);
			void		SetChecked(BOOL bChecked);
			BOOL		IsChecked() const;
		private:
			BOOL			m_bChecked;
			BOOL			m_bEnter;
			TinyImage*		m_images[2];
			TinyVisualMenu* m_child;
		};
	}
}



