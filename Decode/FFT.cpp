#include "stdafx.h"
#include "FFT.h"

namespace Decode
{
	static INT ReverseBits(UINT val)
	{
		UINT reversed = 0;
		for (UINT i = 0; i < FFT_BUFFER_SIZE; i++)
		{
			reversed <<= 1;
			reversed += (val & 1);
			val >>= 1;
		}
		return reversed;
	}

	FFT::FFT()
	{
		for (UINT i = 0; i < FFT_BUFFER_SIZE; i++)
		{
			m_bitReverses[i] = ReverseBits(i);
		}
		for (UINT i = 0; i < FFT_BUFFER_SIZE / 2; i++)
		{
			FLOAT j = static_cast<FLOAT>(2 * PI * i / FFT_BUFFER_SIZE);
			m_costable[i] = cos(j);
			m_sintable[i] = sin(j);
		}
	}
	FFT::~FFT()
	{
	}
	void FFT::Calculate()
	{

	}
	void FFT::Prepare()
	{

	}
}