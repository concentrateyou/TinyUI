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

		class TinyVisualButton;
		/// <summary>
		/// ÏµÍ³°´Å¥
		/// </summary>
		class TinyVisualButton : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualButton)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualButton)
		protected:
			TinyVisualButton();
			TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualButton();
			TinyString RetrieveTag() const OVERRIDE;
			void SetStyleImage(StyleImage type, LPCSTR pzFile);
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		private:
			StyleImage	m_dwStyle;
			TinyImage*	m_images[StyleImage::COUNT];
		};
	}
}



