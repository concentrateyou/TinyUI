#pragma once
#include "DXFramework.h"
#include "Common\TinyUtility.h"
using namespace TinyUI::IO;

class Task : public TinyTaskBase
{
public:
	Task(Closure& invoke);
	~Task();
	BOOL	Submit();
	void	Quit() OVERRIDE;
private:
	void	MessagePump();
private:
	TinyEvent	m_break;
	Closure		m_invoke;
};

