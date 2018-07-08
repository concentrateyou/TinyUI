#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualButton : public TinyVisual
		{
		public:
			enum class ButtonStyle : BYTE
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
			TinyImage*	m_images[static_cast<BYTE>(ButtonStyle::HOVER) + 1];
			ButtonStyle	m_style;
		};
	}
}



