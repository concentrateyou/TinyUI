#pragma once
#include "TinyVisualButton.h"
#include "TinyVisualPopupHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ������ؼ�
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
			BOOL SetStyleImage(StyleImage type, BYTE* ps, DWORD dwSize);
			HRESULT SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags);
		private:
			StyleImage	m_dwFlag;
			TinyImage	m_images[StyleImage::COUNT];
			TinyImage	m_arraws[3];
		};
	}
}



