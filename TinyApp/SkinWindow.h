#pragma once
#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualHWND.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "Control/TinyImageButton.h"
#include "Windowless/TinyVisualComboBox.h"
#include "Windowless/TinyVisualRichText.h"
#include <gdiplus.h>
using namespace TinyUI;
using namespace TinyUI::Windowless;
using namespace Gdiplus;

class SkinWindow : public TinyVisualHWND
{
	DECLARE_DYNAMIC(SkinWindow)
public:
	SkinWindow();
	virtual ~SkinWindow();
public:
	void OnInitialize() OVERRIDE;
	void OnUninitialize() OVERRIDE;
private:
	void OnMinimumClick(EventArgs& args);
	void OnCloseClick(EventArgs& args);
	void OnButtonClick(EventArgs& args);
	void OnSelectChanged(TinyVisualOption* ps);
private:
	TinyScopedPtr<Delegate<void(TinyVisualOption*)>> m_onSelectChanged;
	TinyScopedPtr<Delegate<void(EventArgs&)>> m_onMinimumClick;
	TinyScopedPtr<Delegate<void(EventArgs&)>> m_onCloseClick;
	TinyScopedPtr<Delegate<void(EventArgs&)>> m_onButtonClick;
};

