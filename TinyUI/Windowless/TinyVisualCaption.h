#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ±ÍÃ‚¿∏
		/// </summary>
		class TinyVisualCaption : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualCaption)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualCaption)
		protected:
			TinyVisualCaption(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualCaption();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL IsLayout() const OVERRIDE;
			void OnSizeChange(const TinySize&, const TinySize&) OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		};
	}
}



