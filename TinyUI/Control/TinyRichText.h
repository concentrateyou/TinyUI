#pragma once
#include "../Common/TinyObject.h"
#include "TinyTextHost.h"

namespace TinyUI
{
	class TinyRichText : public TinyObject, public TinyMessageFilter
	{
		DECLARE_DYNAMIC(TinyRichText);
	public:
		TinyRichText();
		~TinyRichText();
		BOOL Initialize();
		BOOL PreTranslateMessage(MSG* pMsg) OVERRIDE;
	private:
		TinyScopedPtr<TinyTextHost> m_texthost;
	};
}