#pragma once
#include "DXFramework.h"
#include "Common/TinyTime.h"
using namespace TinyUI::IO;

class EncodeTask : public TinyTask
{
public:
	EncodeTask();
	~EncodeTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	TinyEvent				m_break;
	TinyPerformanceTimer	m_timer;
};

