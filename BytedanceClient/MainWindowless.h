#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualWindow.h"
using namespace TinyFramework;
using namespace TinyFramework::Windowless;

namespace Bytedance
{
	/// <summary>
	/// Ö÷´°¿Ú
	/// </summary>
	class MainWindowless : public TinyVisualWindowless
	{
		DECLARE_DYNAMIC(MainWindowless)
		DISALLOW_COPY_AND_ASSIGN(MainWindowless)
	public:
		MainWindowless();
		~MainWindowless();
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;
	};
}
