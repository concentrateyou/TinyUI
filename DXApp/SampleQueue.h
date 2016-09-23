#pragma once
#include <deque>
#include "DXFramework.h"
using namespace std;
using namespace TinyUI::IO;

class Sample
{
public:
	BYTE*				Bits;
	INT					INC;
	INT					PTS;
	INT					DTS;
	INT					Type;//0:video.1.audio
};

class SampleQueue
{
public:
	SampleQueue();
	~SampleQueue();
public:
	BOOL	IsEmpty() const;
	void	Add(Sample& sample);
	void	Remove();
	Sample&	GetSample();
private:
	deque<Sample>	m_samples;
	TinyLock		m_lock;
};

