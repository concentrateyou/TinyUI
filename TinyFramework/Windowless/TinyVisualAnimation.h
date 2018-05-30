#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// ¶¯»­¿Ø¼þ
		/// </summary>
		class TinyVisualAnimation : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNCREATE(TinyVisualAnimation)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualAnimation)
		protected:
			TinyVisualAnimation();
			TinyVisualAnimation(TinyVisual* spvisParent, TinyVisualDocument* vtree);
			static VOID CALLBACK OnTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired);
		public:
			virtual ~TinyVisualAnimation();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
		public:
			virtual void	SetAnimateImage(TinyArray<TinyString>& szFiles);
			virtual void	SetAnimateImage(TinyArray<TinyImage*>& images);
			BOOL	BeginAnimate();
			BOOL	EndAnimate();
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		private:
			INT						m_index;
			HANDLE					m_hTimer;
			TinyArray<TinyImage*>	m_images;
		};
	}
}



