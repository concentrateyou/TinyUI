#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualComboBoxItem;
		class TinyVisualComboBoxHWND;

		class TinyVisualComboBox : public TinyVisual
		{
		public:
			enum class ComboBoxStyle : BYTE
			{
				NORMAL = 0,
				PUSH = 1,
				HOVER = 2,
				ARROWNORMAL = 3,
				ARROWDOWN = 4,
				ARROWHOVER = 5
			};
			friend class TinyVisualComboBoxHWND;
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
			TinySize				GetItemSize() const;
			void					SetItemSize(const TinySize& itemsize);
			void					SetSelected(TinyVisualComboBoxItem* item);
			TinyVisualComboBoxItem*	GetSelected();
			TinyVisualComboBoxItem*	Add(const TinyString& name, const TinyString& text);
			void					Remove(const TinyString& name);
			void					RemoveAll();
		public:
			void					SetImage(ComboBoxStyle style, TinyImage* image);
		private:
			TinySize				m_itemsize;
			TinyImage*				m_images[static_cast<BYTE>(ComboBoxStyle::ARROWHOVER) + 1];
			ComboBoxStyle			m_style;
			TinyVisualComboBoxHWND* m_combobox;
		};
	}
}



