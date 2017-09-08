#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "Windowless/TinyVisualNative.h"
#include "Windowless/TinyVisualHWND.h"
#include "Windowless/TinyVisualComboBox.h"
#include "Windowless/TinyVisualTextBox.h"
#include "Media/TinyWASAPIAudioRender.h"
using namespace TinyUI;
using namespace TinyUI::Windowless;
using namespace TinyUI::Media;

namespace MShow
{
	class MShowController;
	/// <summary>
	/// Ö÷´°¿ÚUI
	/// </summary>
	class MShowWindow : public TinyVisualHWND
	{
		friend class MShowController;
		DECLARE_DYNAMIC(MShowWindow)
	public:
		MShowWindow();
		virtual ~MShowWindow();
	public:
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;
	private:
		MPreviewView		m_previewView;
		TinyControl			m_analyserBAR;
	};
}