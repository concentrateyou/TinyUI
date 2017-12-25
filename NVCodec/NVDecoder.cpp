#include "stdafx.h"
#include "NVDecoder.h"

namespace NVCodec
{
	NVDecoder::NVDecoder()
		:m_decoder(NULL)
	{
	}


	NVDecoder::~NVDecoder()
	{
	}

	BOOL NVDecoder::Open(const BYTE* bits, LONG size)
	{
		CUDADRIVER hHandleDriver = 0;
		CUresult cuResult;
		cuResult = cuInit(0, __CUDA_API_VERSION, hHandleDriver);
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;
		cuResult = cuvidInit(0);
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;
		return TRUE;
	_ERROR:
		return FALSE;
	}
	BOOL NVDecoder::Decode(Media::SampleTag& tag)
	{
		return TRUE;
	}
	void NVDecoder::Close()
	{

	}
}
