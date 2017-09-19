#pragma once
#include "TinyVisualButton.h"
#include "TinyVisualComboBoxHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
#define DEFAULT_OPTION_HEIGHT 23

		class TinyVisualComboBoxHWND;
		class TinyVisualComboBox;
		class TinyVisualOption;
		/// <summary>
		/// 下拉框控件
		/// </summary>
		class TinyVisualComboBox : public TinyVisual
		{
			friend class TinyVisualDocument;
			friend class TinyVisualComboBoxHWND;
			DECLARE_DYNCREATE(TinyVisualComboBox)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBox)
		protected:
			TinyVisualComboBox();
			TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualComboBox();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		public:
			TinyVisualOption* AddOption(const TinyString& szValue, const TinyString& szText);
			TinyArray<TinyVisualOption*>& GetOptions();
			void SetStyleImage(StyleImage type, LPCSTR pzFile);
			void SetArrowImage(StyleImage type, LPCSTR pzFile);
			void SetSelected(TinyVisualOption* spvis, BOOL bFlag = TRUE);
			void SetSelected(INT index, BOOL bFlag = TRUE);
			TinyVisualOption* GetSelected();
		public:
			Event<void(TinyVisualOption*)>	EVENT_SELECTCHANGED;
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnInitialize() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
		private:
			void	OnPopupActive(ActiveEventArgs& args);
		private:
			INT						m_cy;
			BOOL					m_bActive;
			StyleImage				m_dwFlag;
			StyleImage				m_dwArrawFlag;
			TinyImage*				m_images[StyleImage::COUNT];
			TinyImage*				m_arraws[StyleImage::COUNT];
			TinyVisualComboBoxHWND  m_popupWND;
			TinyArray<TinyVisualOption*>	m_options;
			TinyScopedPtr<Delegate<void(ActiveEventArgs&)>> m_onPopupActive;
		};
		/// <summary>
		/// 下拉框项
		/// </summary>
		class TinyVisualOption : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualOption)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualOption)
		protected:
			TinyVisualOption();
			TinyVisualOption(TinyVisual* spvisParent, TinyVisualDocument* document);
			virtual ~TinyVisualOption();
			TinyString RetrieveTag() const OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			BOOL	SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		public:
			BOOL IsSelected();
			void SetSelected(BOOL bFlag);
			TinyString GetValue() const;
		public:
			virtual void SetValue(LPCSTR pzValue);
			virtual void SetHighlightImage(LPCSTR pzFile);
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		private:
			TinyString	m_szValue;
			TinyImage*	m_highlight;
			DWORD		m_dwFlag;
			BOOL		m_bSelected;
		};
	}
}