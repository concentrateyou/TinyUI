#pragma once
#include "TinyVisual.h"
#include "TinyVisualWND.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// ±êÌâÀ¸ÈÝÆ÷¿Ø¼þ
		/// </summary>
		class TinyVisualShadow : public TinyVisualWND
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualShadow)
		public:
			TinyVisualShadow();
			virtual ~TinyVisualShadow();
			LPCSTR RetrieveClassName() OVERRIDE;
			LPCSTR RetrieveTitle() OVERRIDE;
			HICON RetrieveIcon() OVERRIDE;
			DWORD RetrieveStyle() OVERRIDE;
			DWORD RetrieveExStyle() OVERRIDE;
		public:
			TinyRectangle GetShadowBox() const;
			void SetShadowBox(const TinyRectangle& box);
			BOOL SetShadow(TinyImage* image);
			BOOL DrawShadow();
		private:
			TinyImage*				m_image;
			TinyRectangle			m_box;
		};
	}
}



