#pragma once
#include "DXFramework.h"
#include "Common\TinyUtility.h"
using namespace TinyUI::IO;

class DXWindow;
class RenderTask : public TinyTaskBase
{
public:
	RenderTask(DXWindow* window,INT frameRate);
	~RenderTask();
	BOOL	Submit();
	void	Exit() OVERRIDE;
private:
	void	MessagePump();
private:
	DXWindow*	m_window;
	TinyEvent	m_event;
	INT			m_frameRate;
};

