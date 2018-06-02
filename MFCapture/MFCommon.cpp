#include "stdafx.h"
#include "MFCommon.h"

namespace MF
{
	BOOL IsYuvPlanar(VideoPixelFormat format)
	{
		switch (format)
		{
		case PIXEL_FORMAT_YV12:
		case PIXEL_FORMAT_I420:
		case PIXEL_FORMAT_YV16:
		case PIXEL_FORMAT_YV12A:
		case PIXEL_FORMAT_YV24:
		case PIXEL_FORMAT_NV12:
		case PIXEL_FORMAT_NV21:
		case PIXEL_FORMAT_MT21:
		case PIXEL_FORMAT_YUV420P9:
		case PIXEL_FORMAT_YUV420P10:
		case PIXEL_FORMAT_YUV422P9:
		case PIXEL_FORMAT_YUV422P10:
		case PIXEL_FORMAT_YUV444P9:
		case PIXEL_FORMAT_YUV444P10:
			return TRUE;
		case PIXEL_FORMAT_UNKNOWN:
		case PIXEL_FORMAT_UYVY:
		case PIXEL_FORMAT_YUY2:
		case PIXEL_FORMAT_ARGB:
		case PIXEL_FORMAT_XRGB:
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_RGB32:
		case PIXEL_FORMAT_MJPEG:
			return FALSE;
		}
		return FALSE;
	}

	BOOL IsOpaque(VideoPixelFormat format)
	{
		switch (format)
		{
		case PIXEL_FORMAT_UNKNOWN:
		case PIXEL_FORMAT_I420:
		case PIXEL_FORMAT_YV12:
		case PIXEL_FORMAT_YV16:
		case PIXEL_FORMAT_YV24:
		case PIXEL_FORMAT_NV12:
		case PIXEL_FORMAT_NV21:
		case PIXEL_FORMAT_UYVY:
		case PIXEL_FORMAT_YUY2:
		case PIXEL_FORMAT_XRGB:
		case PIXEL_FORMAT_RGB24:
		case PIXEL_FORMAT_MJPEG:
		case PIXEL_FORMAT_MT21:
		case PIXEL_FORMAT_YUV420P9:
		case PIXEL_FORMAT_YUV420P10:
		case PIXEL_FORMAT_YUV422P9:
		case PIXEL_FORMAT_YUV422P10:
		case PIXEL_FORMAT_YUV444P9:
		case PIXEL_FORMAT_YUV444P10:
			return TRUE;
		case PIXEL_FORMAT_YV12A:
		case PIXEL_FORMAT_ARGB:
		case PIXEL_FORMAT_RGB32:
			break;
		}
		return FALSE;
	}
	HRESULT CopyAttribute(IMFAttributes *pSrc, IMFAttributes *pDest, const GUID& key)
	{
		PROPVARIANT var;
		PropVariantInit(&var);
		HRESULT hRes = pSrc->GetItem(key, &var);
		if (SUCCEEDED(hRes))
		{
			hRes = pDest->SetItem(key, var);
			PropVariantClear(&var);
		}
		return hRes;
	}

	HRESULT CloneVideoMediaType(IMFMediaType *pSrcMediaType, REFGUID guidSubType, IMFMediaType **ppNewMediaType)
	{
		TinyComPtr<IMFMediaType> newMediaType;
		HRESULT hRes = MFCreateMediaType(&newMediaType);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}

		hRes = newMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}

		hRes = newMediaType->SetGUID(MF_MT_SUBTYPE, guidSubType);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}

		hRes = CopyAttribute(pSrcMediaType, newMediaType, MF_MT_FRAME_SIZE);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}

		hRes = CopyAttribute(pSrcMediaType, newMediaType, MF_MT_FRAME_RATE);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}

		hRes = CopyAttribute(pSrcMediaType, newMediaType, MF_MT_PIXEL_ASPECT_RATIO);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}

		hRes = CopyAttribute(pSrcMediaType, newMediaType, MF_MT_INTERLACE_MODE);
		if (FAILED(hRes))
		{
			goto _ERROR;
		}
		*ppNewMediaType = newMediaType;
		(*ppNewMediaType)->AddRef();
	_ERROR:
		newMediaType.Release();
		return hRes;
	}

}