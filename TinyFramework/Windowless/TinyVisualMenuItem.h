#pragma once
#include "TinyVisualLabel.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualContextMenu;

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
			TinyString		RetrieveTag() const OVERRIDE;
			BOOL			OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT			OnMouseEnter() OVERRIDE;
			HRESULT			OnMouseHover(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT			OnMouseLeave() OVERRIDE;
			HRESULT			OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
		public:
			TinyVisualContextMenu*	GetContextMenu();
			void			SetSeparator(BOOL bSeparator);
			BOOL			IsSeparator() const;
			void			SetChecked(BOOL bChecked);
			BOOL			IsChecked() const;
			void			SetImageList(TinyImage* icon, TinyImage* highlight, TinyImage* check, TinyImage* arrow);
			TinyVisual*		Add();//·Ö¸îÏß
			TinyVisual*		Add(const TinyString& name, const TinyString& text, TinyImage* icon = NULL);
		private:
			DWORD			m_dwFlag;
			TinyImage*		m_images[4];
			TinyVisualContextMenu* m_contextmenu;
		};
	}
}



