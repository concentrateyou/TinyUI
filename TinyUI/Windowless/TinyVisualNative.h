#pragma once
#include "TinyVisual.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// Ô­Éú´°¿Ú
		/// </summary>
		class TinyVisualNative : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualNative)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualNative)
		protected:
			TinyVisualNative(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualNative();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL SetProperty(const TinyString& name, const TinyString& value) OVERRIDE;
			void SetPosition(const TinyPoint& pos) OVERRIDE;
			void SetSize(const TinySize& size) OVERRIDE;
			void SetVisible(BOOL visible) OVERRIDE;
		public:
			void SetNativeWND(TinyControl* control);
			TinyControl* GetNativeWND();
		protected:
			BOOL OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
			HRESULT OnCreate() OVERRIDE;
			HRESULT OnDestory() OVERRIDE;
			HRESULT OnInitialize() OVERRIDE;
			HRESULT OnMouseLeave() OVERRIDE;
			HRESULT	OnLButtonDown(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnMouseMove(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
			HRESULT	OnLButtonUp(const TinyPoint& pos, DWORD dwFlags) OVERRIDE;
		private:
			TinyControl*	m_pWND;
		};
	}
}



