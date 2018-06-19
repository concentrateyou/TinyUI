#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// �����ؼ�
		/// </summary>
		class TinyVisualPanel : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualPanel)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualPanel)
		protected:
			TinyVisualPanel();
			TinyVisualPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualPanel();
			TinyString		RetrieveTag() const OVERRIDE;
			BOOL			SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
			VisualLayout	GetLayout() const;
		protected:
			BOOL			OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			void			OnSizeChange(const TinySize&, const TinySize&) OVERRIDE;
		private:
			VisualLayout	m_layout;
		};
	}
}



