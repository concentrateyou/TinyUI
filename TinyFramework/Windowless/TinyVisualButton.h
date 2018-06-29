#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
#define BST_INCLICK         0x0010
#define BST_CAPTURED        0x0020
#define BST_MOUSE           0x0040
#define BST_DONTCLICK       0x0080
#define BST_INBMCLICK       0x0100

		class TinyVisualButton : public TinyVisual
		{
		public:
			enum class ButtonStyle
			{
				NORMAL = 0,
				DOWN = 1,
				HOVER = 2
			};
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualButton)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualButton)
		protected:
			TinyVisualButton();
			TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualButton();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseLeave() OVERRIDE;
		public:
			void		SetImage(ButtonStyle style, TinyImage* image);
		private:
			TinyImage*	m_images[3];
			ButtonStyle	m_style;
		};
	}
}



