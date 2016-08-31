#pragma once
#include "DXFramework.h"
#include "IO/TinyTaskPool.h"
using namespace TinyUI::IO;

class GraphicsCapture;
class PublishTask : public TinyTask
{
public:
	PublishTask(GraphicsCapture* pSys, TinyTaskPool* pWorks);
	~PublishTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	GraphicsCapture*	m_pSys;
	TinyEvent			m_publish;
};

