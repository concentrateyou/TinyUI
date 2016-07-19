#pragma once
#include <richedit.h>
#include <textserv.h>
#include <imm.h>
#include <tom.h>
#include "TinyVisual.h"
#include "TinyVisualHWND.h"
#include "TinyVisualManage.h"
#include "TinyVisualUtility.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// 无窗口富文本控件
		/// </summary>
		class TinyVisualRichText : public TinyVisual, TinyMessageFilter
		{
			friend class TinyTextHost;
			friend class TinyVisualFactory;
			DECLARE_DYNAMIC(TinyVisualRichText);
			DISALLOW_COPY_AND_ASSIGN(TinyVisualRichText);
		protected:
			TinyVisualRichText(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			virtual ~TinyVisualRichText();
			TinyString RetrieveTag() const OVERRIDE;
			BOOL PreTranslateMessage(MSG* pMsg) OVERRIDE;
		protected:
			LRESULT OnCreate() OVERRIDE;
			LRESULT OnDestory() OVERRIDE;
			BOOL	OnDraw(HDC hDC, const RECT& rcPaint) OVERRIDE;
		public:
			BOOL	SetReadonly(BOOL bReadOnly);
			BOOL	SetPassword(BOOL bPassword);
			BOOL	SetMultiline(BOOL bMultiline);
			BOOL	SetRichText(BOOL bRichText);
			BOOL	SetLimit(DWORD dwLimit);
			BOOL	AllowBeep(BOOL bAllowBeep);
			BOOL	AllowDrag(BOOL bAllowDrag);
		private:
			TinyScopedPtr<TinyTextHost> m_texthost;
			DWORD						m_dwStyle;
			BOOL						m_bRichText;
			BOOL						m_bAllowBeep;
			BOOL						m_bAllowDrag;
		};
	}
}


