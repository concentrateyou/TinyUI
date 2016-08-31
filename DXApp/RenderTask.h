#pragma once
#include "DXFramework.h"
#include "IO/TinyTaskPool.h"
using namespace TinyUI::IO;

class GraphicsCapture;
class RenderTask : public TinyTask
{
public:
	explicit RenderTask(GraphicsCapture* pSys, TinyTaskPool* pWorks);
	~RenderTask();
	BOOL	Submit();
private:
	void	MessagePump();
private:
	GraphicsCapture*	m_pSys;
	TinyEvent			m_render;
};

