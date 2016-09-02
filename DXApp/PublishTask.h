#pragma once
#include "DXFramework.h"
#include "IO/TinyWin32TaskPool.h"
using namespace TinyUI::IO;

class GraphicsCapture;
class PublishTask : public TinyWin32Task
{
public:
	PublishTask(GraphicsCapture* pSys, TinyWin32TaskPool* pWorks);
	~PublishTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	GraphicsCapture*	m_pSys;
	TinyEvent			m_publish;
};

