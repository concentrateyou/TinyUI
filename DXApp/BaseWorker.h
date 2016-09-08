#pragma once
#include "DXFramework.h"
using namespace TinyUI::IO;

class BaseWorker : public TinyTask
{
public:
	BaseWorker(Closure& callback);
	~BaseWorker();
	BOOL	Submit();
	BOOL	Quit();
private:
	void	MessagePump();
private:
	TinyEvent	m_break;
	Closure		m_callback;
};

