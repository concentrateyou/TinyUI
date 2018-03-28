#pragma once
#include "TinyVisual.h"

namespace TinyUI
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
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		};
		/// <summary>
		/// ջ���-ˮƽ�ʹ�ֱ
		/// </summary>
		class TinyVisualStackPanel : public TinyVisualPanel
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualStackPanel)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualStackPanel)
		protected:
			TinyVisualStackPanel();
			TinyVisualStackPanel(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualStackPanel();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		};
	}
}



