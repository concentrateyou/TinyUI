#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualComboBox : public TinyVisual
		{
		public:
			enum class ComboBoxStyle : BYTE
			{
				NORMAL = 0,
				DOWN = 1,
				HOVER = 2,
				ARROWNORMAL = 3,
				ARROWDOWN = 4,
				ARROWHOVER = 5
			};
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualComboBox)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBox)
		protected:
			TinyVisualComboBox();
			TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualComboBox();
			TinyString	RetrieveTag() const OVERRIDE;
			BOOL		OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT		OnLButtonDBClick(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT		OnMouseLeave() OVERRIDE;
		public:
			void		SetImage(ComboBoxStyle style, TinyImage* image);
		private:
			TinyImage*		m_images[static_cast<BYTE>(ComboBoxStyle::ARROWHOVER) + 1];
			ComboBoxStyle	m_style;
		};
	}
}



