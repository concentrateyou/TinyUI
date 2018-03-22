#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "Control/TinyScrollBar.h"
using namespace TinyUI;
using namespace TinyUI::Windowless;

class GLView : public TinyVisualWindowless
{
public:
	GLView();
	virtual ~GLView();
public:
	void OnInitialize() OVERRIDE;
	void OnUninitialize() OVERRIDE;
private:
	void OnCloseClick(TinyVisual*, EventArgs& args);
private:
	TinyScopedPtr<Delegate<void(TinyVisual*, EventArgs&)>>		m_onCloseClick;
};