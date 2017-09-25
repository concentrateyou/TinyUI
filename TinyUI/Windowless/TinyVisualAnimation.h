#pragma once
#include "TinyVisual.h"

namespace TinyUI
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
			virtual void	SetAnimateImage(const TinyString& szName);
			virtual void	SetAnimateImage(TinyImage* image);
			BOOL	BeginAnimate();
			BOOL	EndAnimate();
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		private:
			INT			m_index;
			HANDLE		m_hTimer;
			TinyImage*	m_animation;
		};
	}
}



