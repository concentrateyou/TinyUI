#pragma once
#include "MShowCommon.h"
#include "MPreviewView.h"
#include "Media/TinyWASAPIAudioRender.h"
using namespace TinyUI;
using namespace TinyUI::Windowless;
using namespace TinyUI::Media;

namespace MShow
{
	class MShowController;
	/// <summary>
	/// ¿Í»§¶ËUI
	/// </summary>
	class MClientWindow : public TinyVisualHWND
	{
		friend class MShowController;
		DECLARE_DYNAMIC(MClientWindow)
	public:
		MClientWindow();
		virtual ~MClientWindow();
	public:
		void OnInitialize() OVERRIDE;
		void OnUninitialize() OVERRIDE;
	private:
		MPreviewView		m_previewView;
	};
}