#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualWindow.h"
#include "Windowless/TinyVisualButton.h"
#include "Windowless/TinyVisualLabel.h"
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
	private:
		void BuildUI();
		void Resize(INT cx, INT cy);
	private:
		TinyVisualButton*	m_min;
		TinyVisualButton*	m_max;
		TinyVisualButton*	m_restore;
		TinyVisualButton*	m_close;
		TinyVisualButton*	m_setting;
	};
}
