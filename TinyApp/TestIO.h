#pragma once
#include "Common/TinyUtility.h"
#include "IO/TinyWin32TaskPool.h"
#include "IO/TinyRingQueue.h"
using namespace TinyUI;
using namespace TinyUI::IO;
class TestIO
{
public:
	TestIO();
	~TestIO();
private:
	void Run1();
	void Run2();
private:
	TinyWin32TaskPool	m_taskPool;
	TinyScopedPtr<TinyWin32Task>	m_task1;
	TinyScopedPtr<TinyWin32Task>	m_task2;
	TinyRingQueue					m_queue;
};

