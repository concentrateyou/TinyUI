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