#pragma once
#include "TinyVisual.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisualRichText : public TinyVisual
		{
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualRichText);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualRichText);
		protected:
			TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualRichText();
			TinyString RetrieveTag() const OVERRIDE;
		protected:
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			HRESULT OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnMouseWheel(const TinyPoint& pos, SHORT zDelta, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) OVERRIDE;
			HRESULT OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) OVERRIDE;
			HRESULT OnChar(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags) OVERRIDE;
			HRESULT OnSetCursor(HWND hWND, DWORD dwHitTest, DWORD dwMessage) OVERRIDE;
			HRESULT	OnSetFocus() OVERRIDE;
			HRESULT	OnKillFocus() OVERRIDE;
		private:
			TinyTextHost	m_texthost;
		};
	}
}



