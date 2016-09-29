#pragma once
#include <deque>
#include "DXFramework.h"
using namespace std;
using namespace TinyUI::IO;

class Sample : public TinyReference < Sample >
{
public:
	Sample(DWORD size);
	~Sample();
	void Fill(BYTE* bits,DWORD size);
public:
	TinyScopedPtr<BYTE>	Bits;
	DWORD		Size;
	DWORD		INC;
	DWORD		PTS;
	DWORD		DTS;
	DWORD		Track;//0:video.1.audio
	DWORD		Flag;
};

class SampleQueue
{
public:
	SampleQueue();
	~SampleQueue();
public:
	BOOL	IsEmpty() const;
	void	Add(Sample* sample);
	void	Remove();
	Sample*	GetSample();
private:
	TinyLock								m_lock;
	deque<TinyScopedReferencePtr<Sample>>	m_samples;
};

