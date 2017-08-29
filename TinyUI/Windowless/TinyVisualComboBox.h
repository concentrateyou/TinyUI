#pragma once
#include "TinyVisualButton.h"
#include "TinyVisualDropDownHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 下拉框控件
		/// </summary>
		class TinyVisualComboBox : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualComboBox)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBox)
		protected:
			TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualComboBox();
			TinyString RetrieveTag() const OVERRIDE;
		public:
			BOOL SetStyleImage(StyleImage type, LPCSTR pzFile);
			BOOL SetArrowImage(StyleImage type, LPCSTR pzFile);
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
		private:
			void OnPopupActive(ActiveEventArgs& args);
		private:
			StyleImage				m_dwFlag;
			StyleImage				m_dwArrawFlag;
			TinyImage				m_images[StyleImage::COUNT];
			TinyImage				m_arraws[StyleImage::COUNT];
			TinyString				m_dropdown;
			BOOL					m_bActive;
			TinyVisualDropDownHWND	m_popup;
			TinyScopedArray<Delegate<void(ActiveEventArgs&)>> m_onPopupActive;
		};
		/// <summary>
		/// 下拉框项
		/// </summary>
		class TinyVisualOption : public TinyVisual
		{

		};
	}
}