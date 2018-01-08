#include "stdafx.h"
#include "NVDecoder.h"

namespace NVCodec
{
	NVDecoder::NVDecoder()
		:m_decoder(NULL),
		m_parser(NULL),
		m_videolock(NULL),
		m_context(NULL)
	{
		ZeroMemory(&m_createINFO, sizeof(m_createINFO));
		ZeroMemory(&m_videoFMT, sizeof(m_videoFMT));
	}

	NVDecoder::~NVDecoder()
	{
	}

	BOOL NVDecoder::Open(const TinySize& targetSize, const BYTE* bits, LONG size)
	{
		CUDADRIVER hHandleDriver = 0;
		CUresult cuResult;
		cuResult = cuInit(0, __CUDA_API_VERSION, hHandleDriver);
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;
		cuResult = cuvidInit(0);
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;
		/*cuResult = cuvidCtxLockCreate();
		if (cuResult != CUDA_SUCCESS)
			goto _ERROR;*/
		return TRUE;
	_ERROR:
		return FALSE;
	}
	BOOL NVDecoder::Decode(Media::SampleTag& tag)
	{
		CUVIDSOURCEDATAPACKET packet = { 0 };
		packet.payload_size = tag.size;
		packet.payload = tag.bits;
		packet.timestamp = static_cast<CUvideotimestamp>(tag.timestamp);
		return TRUE;
	}
	void NVDecoder::Close()
	{

	}
}
