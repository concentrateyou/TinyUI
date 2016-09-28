#include "stdafx.h"
#include "TestIO.h"

TestIO::TestIO()
{
	m_taskPool.Initialize(3, 3);
	m_task1.Reset(new TinyWin32Task(&m_taskPool));
	m_task2.Reset(new TinyWin32Task(&m_taskPool));
	m_queue.Initialize(32);

	Closure s1 = BindCallback(&TestIO::Run1, this);
	m_task1->Submit(s1);
	Closure s2 = BindCallback(&TestIO::Run2, this);
	m_task2->Submit(s2);
}

void TestIO::Run1()
{
	for (INT i = 0; i < 100; i++)
	{
		INT s = i + 100;
		BYTE bytes[4];
		bytes[0] = (BYTE)(s & 0xff);
		bytes[1] = (BYTE)((s >> 8) & 0xff);
		bytes[2] = (BYTE)((s >> 16) & 0xff);
		bytes[3] = (BYTE)((s >> 24) & 0xff);
		UINT is = m_queue.Write(bytes, 4);
		Sleep(2000);
	}
}

void TestIO::Run2()
{
	for (;;)
	{
		BYTE bytes[4];
		UINT is = m_queue.Read(bytes, 4);
		UINT s = (UINT)bytes[0] & 0xff;
		s |= (((UINT)bytes[1] << 8) & 0xff00);
		s |= (((UINT)bytes[2] << 16) & 0xff0000);
		s |= (((UINT)bytes[3] << 24) & 0xff000000);
		Sleep(4000);
	}
}

TestIO::~TestIO()
{

}
