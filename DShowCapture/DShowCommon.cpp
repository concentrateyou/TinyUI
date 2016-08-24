#include "stdafx.h"
#include "DShowCommon.h"

GUID MediaSubTypeI420 =
{
	0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 }
};
GUID MediaSubTypeHDYC =
{
	0x43594448, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 }
};
const REFERENCE_TIME SecondsToReferenceTime = 10000000;

HRESULT WINAPI CopyMediaType(AM_MEDIA_TYPE *pmtTarget, const AM_MEDIA_TYPE *pmtSource)
{
	ASSERT(pmtSource != pmtTarget);
	*pmtTarget = *pmtSource;
	if (pmtSource->cbFormat != NULL)
	{
		ASSERT(pmtSource->pbFormat != NULL);
		pmtTarget->pbFormat = (PBYTE)CoTaskMemAlloc(pmtSource->cbFormat);
		if (pmtTarget->pbFormat == NULL)
		{
			pmtTarget->cbFormat = 0;
			return E_OUTOFMEMORY;
		}
		else
		{
			CopyMemory((PVOID)pmtTarget->pbFormat, (PVOID)pmtSource->pbFormat, pmtTarget->cbFormat);
		}
	}
	if (pmtTarget->pUnk != NULL)
	{
		pmtTarget->pUnk->AddRef();
	}
	return NOERROR;
}
void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}
void WINAPI DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	if (pmt)
	{
		FreeMediaType(*pmt);
		CoTaskMemFree((PVOID)pmt);
	}
}
//////////////////////////////////////////////////////////////////////////
namespace Media
{
	CaptureParam::CaptureParam()
		:m_rate(0.0F)
	{

	}
	CaptureParam::CaptureParam(FLOAT rate)
		: m_rate(rate)
	{

	}
	CaptureParam::~CaptureParam()
	{

	}
	void CaptureParam::SetRate(FLOAT rate)
	{
		m_rate = rate;
	}
	FLOAT CaptureParam::GetRate() const
	{
		return m_rate;
	}
}