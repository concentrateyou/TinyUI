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
#include "Windowless/TinyVisualTextBox.h"
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
	void OnMinimumClick(TinyVisual*, EventArgs& args);
	void OnCloseClick(TinyVisual*, EventArgs& args);
	void OnButtonClick(TinyVisual*, EventArgs& args);
	void OnSelectChanged(TinyVisualOption* ps);
private:
	TinyScopedPtr<Delegate<void(TinyVisualOption*)>> m_onSelectChanged;
	TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>> m_onMinimumClick;
	TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>> m_onCloseClick;
	TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>> m_onButtonClick;
};

