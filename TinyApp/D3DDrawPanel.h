#pragma once
#include "Control/TinyControl.h"
#pragma comment(lib,"TinyUI.lib")
using namespace TinyUI;

class CD3DDrawPanel : public TinyControl
{
	DECLARE_DYNAMIC(CD3DDrawPanel)
public:
	CD3DDrawPanel();
	~CD3DDrawPanel();
};

