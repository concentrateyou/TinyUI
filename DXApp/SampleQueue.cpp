#include "stdafx.h"
#include "SampleQueue.h"


Sample::Sample(DWORD size)
	:Size(size)
{
	Bits.Reset(new BYTE[Size]);
}
Sample::~Sample()
{

}
void Sample::Fill(BYTE* bits, DWORD size)
{
	ASSERT(Size == size);
	memcpy(Bits, bits, size);
}
//////////////////////////////////////////////////////////////////////////
RawSample::RawSample(DWORD size)
	:Size(size)
{
	Bits.Reset(new BYTE[Size]);
	Time = timeGetTime();
}
RawSample::~RawSample()
{

}
void RawSample::Fill(BYTE* bits, DWORD size)
{
	ASSERT(Size == size);
	memcpy(Bits, bits, size);
}
//////////////////////////////////////////////////////////////////////////
RawSampleQueue::RawSampleQueue()
{
}


RawSampleQueue::~RawSampleQueue()
{
}

BOOL RawSampleQueue::IsEmpty() const
{
	return m_samples.empty();
}

void RawSampleQueue::Add(RawSample* sample)
{
	TinyAutoLock lock(m_lock);
	m_samples.push_back(sample);
}
void RawSampleQueue::Remove()
{
	TinyAutoLock lock(m_lock);
	m_samples.pop_front();
}

RawSample*	RawSampleQueue::GetSample()
{
	return m_samples.front();
}
//////////////////////////////////////////////////////////////////////////
SampleQueue::SampleQueue()
{
}


SampleQueue::~SampleQueue()
{
}

BOOL SampleQueue::IsEmpty() const
{
	return m_samples.empty();
}

void SampleQueue::Add(Sample* sample)
{
	TinyAutoLock lock(m_lock);
	switch (sample->Track)
	{
	case 0://Video
		break;
	case 1://Audio
		break;
	}
	m_samples.push_back(sample);
}
void SampleQueue::Remove()
{
	TinyAutoLock lock(m_lock);
	m_samples.pop_front();
}

Sample*	SampleQueue::GetSample()
{
	return m_samples.front();
}