#include "stdafx.h"
#include "TestIO.h"

TestIO::TestIO()
{
	m_taskPool.Initialize(3, 3);
	m_task1.Reset(new TinyWin32Task(&m_taskPool));
	m_task2.Reset(new TinyWin32Task(&m_taskPool));
	m_queue.Initialize(8);
	Closure s1 = BindCallback(&TestIO::Run1, this);
	m_task1->Submit(s1);
	Closure s2 = BindCallback(&TestIO::Run2, this);
	m_task2->Submit(s2);
}

void TestIO::Run1()
{
	for (;;)
	{
		INT s = RandInteger(100, 1000);
		BYTE bytes[4];
		bytes[0] = (BYTE)(s);
		bytes[1] = (BYTE)(s >> 8);
		bytes[2] = (BYTE)(s >> 16);
		bytes[3] = (BYTE)(s >> 24);
		UINT is = m_queue.Write(bytes, 4);
		TRACE("TestIO-Run1:%d,%d\n", s, is);
		Sleep(50);
	}
}

void TestIO::Run2()
{
	for (;;)
	{
		BYTE bytes[4];
		UINT is = m_queue.Read(bytes, 4);
		INT s = (bytes[0]) | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
		TRACE("TestIO-Run2:%d,%d\n", s, is);
		Sleep(100);
	}
}

TestIO::~TestIO()
{

}
