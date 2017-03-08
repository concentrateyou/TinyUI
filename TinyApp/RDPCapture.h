#pragma once
#include "Render/TinyGDI.h"

class RDPCapture
{
	int mw;
	int mh;
	int mx0;
	int my0;
	DEVMODE myDM;
	DISPLAY_DEVICE myDev;
	HDC m_driverDC;
	BITMAPINFO	m_BmpInfo;
	HBITMAP	m_Bitmap;
	HBITMAP	Old_bitmap;
	DEVMODE oldDM;
public:
	RDPCapture(DISPLAY_DEVICE dev, DEVMODE dm);
	~RDPCapture();
	virtual bool Init(int x0, int y0, int width, int height);
	virtual bool Fill(HDC hDstDC);
};
