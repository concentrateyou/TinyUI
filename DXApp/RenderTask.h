#pragma once
#include "DXFramework.h"
#include "IO/TinyWin32TaskPool.h"
using namespace TinyUI::IO;

class GraphicsCapture;
class RenderTask : public TinyWin32Task
{
public:
	RenderTask(GraphicsCapture* pSys, TinyWin32TaskPool* pWorks);
	~RenderTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	GraphicsCapture*	m_pSys;
	TinyEvent			m_render;
};

