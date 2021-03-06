#pragma once
#pragma once
#include "Common/TinyModule.h"
#include "Windowless/TinyVisualWindowless.h"
#include "Windowless/TinyVisualWindow.h"
#include "Control/TinyComboBox.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyButton.h"
#include "Control/TinyLabel.h"
#include "Control/TinyRichTextBox.h"
#include "Control/TinyImageButton.h"
#include <gdiplus.h>
using namespace TinyFramework;
using namespace TinyFramework::Windowless;
using namespace Gdiplus;

class SkinWindow : public TinyVisualWindowless
{
	DECLARE_DYNAMIC(SkinWindow)
public:
	SkinWindow();
	virtual ~SkinWindow();
	void OnInitialize() OVERRIDE;
	void OnUninitialize() OVERRIDE;
private:
	BOOL BuildUI();
	void Resize(const TinySize& size);
};

