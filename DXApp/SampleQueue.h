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
	DWORD		Time;
	DWORD		Track;//0:video.1.audio
	DWORD		Flag;
};

class RawSample : public TinyReference < RawSample >
{
public:
	RawSample(DWORD size);
	~RawSample();
	TinyScopedPtr<BYTE>	Bits;
	LONG	Size;
	DWORD	Time;
	DWORD	SampleTime;
	void	Fill(BYTE* bits, DWORD size);
};

class RawSampleQueue
{
public:
	RawSampleQueue();
	~RawSampleQueue();
public:
	size_t  GetSize() const;
	BOOL	IsEmpty() const;
	void	Add(RawSample* sample);
	void	Remove();
	RawSample*	GetSample();
private:
	TinyLock									m_lock;
	deque<TinyScopedReferencePtr<RawSample>>	m_samples;
};


class SampleQueue
{
public:
	SampleQueue();
	~SampleQueue();
public:
	size_t  GetSize() const;
	BOOL	IsEmpty() const;
	void	Add(Sample* sample);
	void	Remove();
	Sample*	GetSample();
private:
	TinyLock								m_lock;
	deque<TinyScopedReferencePtr<Sample>>	m_samples;
};

