#include "stdafx.h"
#include "RDPCapture.h"


RDPCapture::RDPCapture(DISPLAY_DEVICE dev, DEVMODE dm)
{
	myDev = dev;
	myDM = dm;
	oldDM = dm;
	m_driverDC = NULL;
}
RDPCapture::~RDPCapture()
{
	if (m_driverDC != NULL) 
		DeleteDC(m_driverDC);
	oldDM.dmDeviceName[0] = 0;
	ChangeDisplaySettingsEx(myDev.DeviceName, &oldDM, 0, 0, 0);
}
bool RDPCapture::Init(int x0, int y0, int width, int height)
{
	mx0 = x0;
	my0 = y0;
	mw = (width + 3) & 0xFFFC;
	mh = height;

	DEVMODE dm;
	dm = myDM;
	WORD drvExtraSaved = dm.dmDriverExtra;
	memset(&dm, 0, sizeof(DEVMODE));
	dm.dmSize = sizeof(DEVMODE);
	dm.dmDriverExtra = drvExtraSaved;
	dm.dmPelsWidth = 1920;
	dm.dmPelsHeight = 1080;
	dm.dmBitsPerPel = 24;
	dm.dmPosition.x = 0;
	dm.dmPosition.y = 0;
	dm.dmDeviceName[0] = '\0';
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_POSITION;
	if (ChangeDisplaySettingsEx(myDev.DeviceName, &dm, 0, CDS_UPDATEREGISTRY, 0))
	{
		ChangeDisplaySettingsEx(myDev.DeviceName, &dm, 0, 0, 0);
	}
	//------------------------------------------------
	ZeroMemory(&m_BmpInfo, sizeof(BITMAPINFO));
	m_BmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfo.bmiHeader.biBitCount = 24;
	m_BmpInfo.bmiHeader.biCompression = BI_RGB;
	m_BmpInfo.bmiHeader.biPlanes = 1;
	m_BmpInfo.bmiHeader.biWidth = mw;
	m_BmpInfo.bmiHeader.biHeight = -mh;
	m_BmpInfo.bmiHeader.biSizeImage = mw*mh * 3;
	m_driverDC = CreateDC(myDev.DeviceName, 0, 0, 0);
	return true;
};

bool RDPCapture::Fill(HDC hDstDC)
{
	BitBlt(hDstDC, 0, 0, mw, mh, m_driverDC, mx0, my0, SRCCOPY | CAPTUREBLT);
	return true;
}
