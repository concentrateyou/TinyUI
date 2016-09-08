#pragma once
#include "DXFramework.h"
using namespace TinyUI::IO;

class GraphicsCapture;
class PublishTask : public TinyTask
{
public:
	PublishTask(GraphicsCapture* pSys);
	~PublishTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	GraphicsCapture*	m_pSys;

	TinyEvent			m_break;
};

