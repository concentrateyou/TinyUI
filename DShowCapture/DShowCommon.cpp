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

HRESULT WINAPI AMovieSetupRegisterFilter(const AMOVIESETUP_FILTER * const psetupdata, IFilterMapper* pIFM, BOOL bRegister)
{
	if (NULL == psetupdata)
		return S_FALSE;
	HRESULT hRes = pIFM->UnregisterFilter(*(psetupdata->clsID));
	if (bRegister)
	{
		hRes = pIFM->RegisterFilter(*(psetupdata->clsID)
			, psetupdata->strName
			, psetupdata->dwMerit);
		if (SUCCEEDED(hRes))
		{
			for (UINT m1 = 0; m1 < psetupdata->nPins; m1++)
			{
				hRes = pIFM->RegisterPin(*(psetupdata->clsID)
					, psetupdata->lpPin[m1].strName
					, psetupdata->lpPin[m1].bRendered
					, psetupdata->lpPin[m1].bOutput
					, psetupdata->lpPin[m1].bZero
					, psetupdata->lpPin[m1].bMany
					, *(psetupdata->lpPin[m1].clsConnectsToFilter)
					, psetupdata->lpPin[m1].strConnectsToPin);

				if (SUCCEEDED(hRes))
				{
					for (UINT m2 = 0; m2 < psetupdata->lpPin[m1].nMediaTypes; m2++)
					{
						hRes = pIFM->RegisterPinType(*(psetupdata->clsID)
							, psetupdata->lpPin[m1].strName
							, *(psetupdata->lpPin[m1].lpMediaType[m2].clsMajorType)
							, *(psetupdata->lpPin[m1].lpMediaType[m2].clsMinorType));
						if (FAILED(hRes)) break;
					}
					if (FAILED(hRes)) break;
				}
				if (FAILED(hRes)) break;
			}
		}
	}
	if (0x80070002 == hRes)
		return NOERROR;
	else
		return hRes;
}

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
void WINAPI SaveBitmap(const BITMAPINFOHEADER& bi, const BYTE* pBits, DWORD dwSize)
{
	BITMAPFILEHEADER  bmfHeader = { 0 };
	DWORD dwSizeofDIB = dwSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfSize = dwSizeofDIB;
	bmfHeader.bfType = 0x4D42;
	HANDLE hFile = CreateFile("D:\\test.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)pBits, dwSize, &dwBytesWritten, NULL);
	CloseHandle(hFile);
}
BOOL WINAPI SupportsPropertyPage(IBaseFilter *pFilter)
{
	if (!pFilter)
		return FALSE;
	HRESULT hRes = S_OK;
	TinyComPtr<ISpecifyPropertyPages> specify;
	hRes = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&specify);
	return SUCCEEDED(hRes);
}
HRESULT WINAPI ShowFilterPropertyPage(IBaseFilter *pFilter, HWND hwndParent)
{
	if (!pFilter)
		return E_NOINTERFACE;
	HRESULT hRes = S_OK;
	TinyComPtr<ISpecifyPropertyPages> specify;
	hRes = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&specify);
	if (SUCCEEDED(hRes))
	{
		do
		{
			FILTER_INFO FilterInfo;
			hRes = pFilter->QueryFilterInfo(&FilterInfo);
			if (FAILED(hRes))
				break;
			CAUUID caGUID;
			hRes = specify->GetPages(&caGUID);
			if (FAILED(hRes))
				break;
			OleCreatePropertyFrame(
				hwndParent,
				0,
				0,
				FilterInfo.achName,
				1,
				(IUnknown **)&pFilter,
				caGUID.cElems,
				caGUID.pElems,
				0,
				0,
				NULL
			);
			CoTaskMemFree(caGUID.pElems);
			FilterInfo.pGraph->Release();

		} while (0);
	}
	pFilter->Release();
	return hRes;
}
HBITMAP WINAPI CopyScreenToBitmap(LPRECT lpRect, BYTE *pData, BITMAPINFO *pHeader)
{
	HDC         hScrDC, hMemDC;
	HBITMAP     hBitmap, hOldBitmap;
	int         nX, nY, nX2, nY2;
	int         nWidth, nHeight;
	int         xScrn, yScrn;
	if (IsRectEmpty(lpRect))
		return NULL;
	hScrDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	hMemDC = CreateCompatibleDC(hScrDC);
	nX = lpRect->left;
	nY = lpRect->top;
	nX2 = lpRect->right;
	nY2 = lpRect->bottom;
	xScrn = GetDeviceCaps(hScrDC, HORZRES);
	yScrn = GetDeviceCaps(hScrDC, VERTRES);
	if (nX < 0)
		nX = 0;
	if (nY < 0)
		nY = 0;
	if (nX2 > xScrn)
		nX2 = xScrn;
	if (nY2 > yScrn)
		nY2 = yScrn;

	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	GetDIBits(hScrDC, hBitmap, 0, nHeight, pData, pHeader, DIB_RGB_COLORS);
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	return hBitmap;
}