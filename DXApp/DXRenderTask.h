#pragma once
#include "DXFramework.h"
#include "IO/TinyTaskPool.h"
using namespace TinyUI::IO;

class DXSystem;
class DXRenderTask : public TinyTask
{
public:
	explicit DXRenderTask(DXSystem* pSys,TinyTaskPool* pWorks);
	~DXRenderTask();
	BOOL	Submit();
	void	Cancel();
private:
	void	MessagePump();
private:
	DXSystem*	m_pSys;
	BOOL		m_bCancel;
};

