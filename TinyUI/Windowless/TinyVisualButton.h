#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ÏµÍ³°´Å¥
		/// </summary>
		class TinyVisualButton : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualButton)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualButton)
		protected:
			TinyVisualButton(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualButton();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL SetStyleImage(StyleImage type, LPCSTR pzFile);
			BOOL SetStyleImage(StyleImage type, BYTE*ps, DWORD dwSize);

			HRESULT SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;

		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		private:
			StyleImage	m_dwFlag;
			TinyImage	m_images[4];
		};
	}
}



