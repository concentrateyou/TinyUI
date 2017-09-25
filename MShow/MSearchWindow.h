#pragma once
#include "MShowCommon.h"
using namespace TinyUI;
using namespace TinyUI::Windowless;
using namespace TinyUI::Media;

namespace MShow
{
	/// <summary>
	/// ������UI
	/// </summary>
	class MSearchWindow : public TinyVisualHWND
	{
		friend class MSearchController;
		DECLARE_DYNAMIC(MSearchWindow)
	public:
		MSearchWindow();
		virtual ~MSearchWindow();
	public:
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;
	};
}


