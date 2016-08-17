#pragma once
#include "TinyVisualButton.h"
#include "TinyVisualDropDownHWND.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// ÏÂÀ­¿ò¿Ø¼þ
		/// </summary>
		class TinyVisualComboBox : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualComboBox);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualComboBox);
		protected:
			TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* document);
		public:
			virtual ~TinyVisualComboBox();
			TinyString RetrieveTag() const OVERRIDE;
		public:
			
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT	OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
		private:
			void OnInputClick(EventArgs& args);
		private:
			TinyScopedPtr<Delegate<void(EventArgs&)>> m_onClick;
			TinyVisualButton*	m_input;
		};
	}
}



