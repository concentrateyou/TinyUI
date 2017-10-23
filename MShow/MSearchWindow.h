#pragma once
#include "MShowCommon.h"
using namespace TinyUI;
using namespace TinyUI::Windowless;
using namespace TinyUI::Media;

namespace MShow
{
	/// <summary>
	/// Ö÷´°¿ÚUI
	/// </summary>
	class MSearchWindow : public TinyVisualHWND
	{
		friend class MSearchController;
		DECLARE_DYNAMIC(MSearchWindow)
	public:
		MSearchWindow();
		virtual ~MSearchWindow();
		LPCSTR RetrieveClassName() OVERRIDE;
		LPCSTR RetrieveTitle() OVERRIDE;
	public:
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;
	};
}


