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
private:
	TinyScopedPtr<Delegate<void(EventArgs&)>> m_onMinimumClick;
	TinyScopedPtr<Delegate<void(EventArgs&)>> m_onCloseClick;
};

