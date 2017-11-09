#include "../stdafx.h"
#include "TinyHandleMap.h"
#include "TinyApplication.h"

namespace TinyUI
{


	IMPLEMENT_DYNAMIC(TinyHandleHWND, TinyObject);

	TinyHandleHWND::TinyHandleHWND()
		:m_hWND(NULL)
	{

	}
	TinyHandleHWND::~TinyHandleHWND()
	{

	}
	TinyHandleHWND::operator HWND() const
	{
		return this == NULL ? NULL : m_hWND;
	}
	HWND TinyHandleHWND::Handle() const
	{
		return this == NULL ? NULL : m_hWND;
	}
	BOOL TinyHandleHWND::operator == (const TinyHandleHWND& obj) const
	{
		return obj.m_hWND == m_hWND;
	}
	BOOL TinyHandleHWND::operator != (const TinyHandleHWND& obj) const
	{
		return obj.m_hWND != m_hWND;
	}
	BOOL TinyHandleHWND::Attach(HWND hWND)
	{
		if (hWND == NULL)
		{
			return FALSE;
		}
		m_hWND = hWND;
		TinyHandleHWND* ps = this;
		m_map.Add(m_hWND, this);
		return TRUE;
	}
	HWND TinyHandleHWND::Detach()
	{
		HWND hWND = m_hWND;
		if (hWND != NULL)
		{
			m_map.Remove(m_hWND);
		}
		m_hWND = NULL;
		return hWND;
	}
	TinyHandleHWND* TinyHandleHWND::Find(HWND hWND)
	{
		return *m_map.Find(hWND);
	}
	TinyHandleMap<HWND, TinyHandleHWND*> TinyHandleHWND::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHMENU, TinyObject);
	TinyHandleHMENU::TinyHandleHMENU()
		:m_hMENU(NULL)
	{

	}
	TinyHandleHMENU::~TinyHandleHMENU()
	{

	}
	TinyHandleHMENU::operator HMENU() const
	{
		return this == NULL ? NULL : m_hMENU;
	}
	HMENU TinyHandleHMENU::Handle() const
	{
		return this == NULL ? NULL : m_hMENU;
	}
	BOOL TinyHandleHMENU::operator == (const TinyHandleHMENU& obj) const
	{
		return obj.m_hMENU == m_hMENU;
	}
	BOOL TinyHandleHMENU::operator != (const TinyHandleHMENU& obj) const
	{
		return obj.m_hMENU != m_hMENU;
	}
	BOOL TinyHandleHMENU::Attach(HMENU hMENU)
	{
		if (hMENU == NULL)
		{
			return FALSE;
		}
		m_hMENU = hMENU;
		TinyHandleHMENU* ps = this;
		m_map.Add(m_hMENU, ps);
		return TRUE;
	}
	HMENU TinyHandleHMENU::Detach()
	{
		HMENU hMENU = m_hMENU;
		if (hMENU != NULL)
		{
			m_map.Remove(m_hMENU);
		}
		m_hMENU = NULL;
		return hMENU;
	}
	TinyHandleHMENU* TinyHandleHMENU::Find(HMENU hMENU)
	{
		return *m_map.Find(hMENU);
	}
	TinyHandleMap<HMENU, TinyHandleHMENU*> TinyHandleHMENU::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHDC, TinyObject);
	TinyHandleHDC::TinyHandleHDC()
		:m_hDC(NULL)
	{

	}
	TinyHandleHDC::~TinyHandleHDC()
	{
		m_hDC = NULL;
	}
	TinyHandleHDC::operator HDC() const
	{
		return this == NULL ? NULL : m_hDC;
	}
	HDC TinyHandleHDC::Handle() const
	{
		return this == NULL ? NULL : m_hDC;
	}
	BOOL TinyHandleHDC::operator == (const TinyHandleHDC& obj) const
	{
		return obj.m_hDC == m_hDC;
	}
	BOOL TinyHandleHDC::operator != (const TinyHandleHDC& obj) const
	{
		return obj.m_hDC != m_hDC;
	}
	BOOL TinyHandleHDC::Attach(HDC hDC)
	{
		if (!hDC)
			return FALSE;
		m_hDC = hDC;
		TinyHandleHDC* ps = this;
		m_map.Add(m_hDC, ps);
		return TRUE;
	}
	HDC TinyHandleHDC::Detach()
	{
		HDC hDC = m_hDC;
		if (hDC != NULL)
		{
			m_map.Remove(m_hDC);
		}
		m_hDC = NULL;
		return hDC;
	}
	TinyHandleHDC* TinyHandleHDC::Find(HDC hDC)
	{
		return *m_map.Find(hDC);
	}
	TinyHandleMap<HDC, TinyHandleHDC*> TinyHandleHDC::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHBITMAP, TinyObject)
		TinyHandleHBITMAP::TinyHandleHBITMAP()
		:m_hBITMAP(NULL)
	{

	}
	TinyHandleHBITMAP::~TinyHandleHBITMAP()
	{

	}
	TinyHandleHBITMAP::operator HBITMAP() const
	{
		return this == NULL ? NULL : m_hBITMAP;
	}
	HBITMAP TinyHandleHBITMAP::Handle() const
	{
		return this == NULL ? NULL : m_hBITMAP;
	}
	BOOL TinyHandleHBITMAP::operator == (const TinyHandleHBITMAP& obj) const
	{
		return obj.m_hBITMAP == m_hBITMAP;
	}
	BOOL TinyHandleHBITMAP::operator != (const TinyHandleHBITMAP& obj) const
	{
		return obj.m_hBITMAP != m_hBITMAP;
	}
	BOOL TinyHandleHBITMAP::Attach(HBITMAP hBITMAP)
	{
		if (!hBITMAP)
			return FALSE;
		m_hBITMAP = hBITMAP;
		TinyHandleHBITMAP* ps = this;
		m_map.Add(m_hBITMAP, ps);
		return TRUE;
	}
	HBITMAP TinyHandleHBITMAP::Detach()
	{
		HBITMAP hBITMAP = m_hBITMAP;
		if (hBITMAP != NULL)
		{
			m_map.Remove(m_hBITMAP);
		}
		m_hBITMAP = NULL;
		return hBITMAP;
	}
	TinyHandleHBITMAP* TinyHandleHBITMAP::Find(HBITMAP hBITMAP)
	{
		TinyHandleHBITMAP** ps = m_map.Find(hBITMAP);
		if (ps != NULL)
		{
			return *ps;
		}
		return NULL;
	}
	TinyHandleMap<HBITMAP, TinyHandleHBITMAP*> TinyHandleHBITMAP::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHFONT, TinyObject);
	TinyHandleHFONT::TinyHandleHFONT()
		:m_hFONT(NULL)
	{

	}
	TinyHandleHFONT::~TinyHandleHFONT()
	{

	}
	TinyHandleHFONT::operator HFONT() const
	{
		return this == NULL ? NULL : m_hFONT;
	}
	HFONT TinyHandleHFONT::Handle() const
	{
		return this == NULL ? NULL : m_hFONT;
	}
	BOOL TinyHandleHFONT::operator == (const TinyHandleHFONT& obj) const
	{
		return obj.m_hFONT == m_hFONT;
	}
	BOOL TinyHandleHFONT::operator != (const TinyHandleHFONT& obj) const
	{
		return obj.m_hFONT != m_hFONT;
	}
	BOOL TinyHandleHFONT::Attach(HFONT hFONT)
	{
		if (!hFONT)
			return FALSE;
		m_hFONT = hFONT;
		TinyHandleHFONT* ps = this;
		m_map.Add(m_hFONT, ps);
		return TRUE;
	}
	HFONT TinyHandleHFONT::Detach()
	{
		HFONT hFONT = m_hFONT;
		if (hFONT != NULL)
		{
			m_map.Remove(m_hFONT);
		}
		m_hFONT = NULL;
		return hFONT;
	}
	TinyHandleHFONT* TinyHandleHFONT::Find(HFONT hFONT)
	{
		return *m_map.Find(hFONT);
	}
	TinyHandleMap<HFONT, TinyHandleHFONT*> TinyHandleHFONT::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHPEN, TinyObject);
	TinyHandleHPEN::TinyHandleHPEN()
		:m_hPEN(NULL)
	{

	}
	TinyHandleHPEN::~TinyHandleHPEN()
	{

	}
	TinyHandleHPEN::operator HPEN() const
	{
		return this == NULL ? NULL : m_hPEN;
	}
	HPEN TinyHandleHPEN::Handle() const
	{
		return this == NULL ? NULL : m_hPEN;
	}
	BOOL TinyHandleHPEN::operator == (const TinyHandleHPEN& obj) const
	{
		return obj.m_hPEN == m_hPEN;
	}
	BOOL TinyHandleHPEN::operator != (const TinyHandleHPEN& obj) const
	{
		return obj.m_hPEN != m_hPEN;
	}
	BOOL TinyHandleHPEN::Attach(HPEN hPEN)
	{
		if (!hPEN)
			return FALSE;
		m_hPEN = hPEN;
		TinyHandleHPEN* ps = this;
		m_map.Add(hPEN, ps);
		return TRUE;
	}
	HPEN TinyHandleHPEN::Detach()
	{
		HPEN hPEN = m_hPEN;
		if (hPEN != NULL)
		{
			m_map.Remove(m_hPEN);
		}
		m_hPEN = NULL;
		return hPEN;
	}
	TinyHandleHPEN* TinyHandleHPEN::Find(HPEN hPEN)
	{
		TinyHandleHPEN** ps = m_map.Find(hPEN);
		if (ps != NULL)
		{
			return *ps;
		}
		return NULL;
	}
	TinyHandleMap<HPEN, TinyHandleHPEN*> TinyHandleHPEN::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHBRUSH, TinyObject);
	TinyHandleHBRUSH::TinyHandleHBRUSH()
		:m_hBRUSH(NULL)
	{

	}
	TinyHandleHBRUSH::~TinyHandleHBRUSH()
	{

	}
	TinyHandleHBRUSH::operator HBRUSH() const
	{
		return this == NULL ? NULL : m_hBRUSH;
	}
	HBRUSH TinyHandleHBRUSH::Handle() const
	{
		return this == NULL ? NULL : m_hBRUSH;
	}
	BOOL TinyHandleHBRUSH::operator == (const TinyHandleHBRUSH& obj) const
	{
		return obj.m_hBRUSH == m_hBRUSH;
	}
	BOOL TinyHandleHBRUSH::operator != (const TinyHandleHBRUSH& obj) const
	{
		return obj.m_hBRUSH != m_hBRUSH;
	}
	BOOL TinyHandleHBRUSH::Attach(HBRUSH hBRUSH)
	{
		if (!hBRUSH)
			return FALSE;
		m_hBRUSH = hBRUSH;
		TinyHandleHBRUSH* ps = this;
		m_map.Add(hBRUSH, ps);
		return TRUE;
	}
	HBRUSH TinyHandleHBRUSH::Detach()
	{
		HBRUSH hBRUSH = m_hBRUSH;
		if (hBRUSH != NULL)
		{
			m_map.Remove(hBRUSH);
		}
		m_hBRUSH = NULL;
		return hBRUSH;
	}
	TinyHandleHBRUSH* TinyHandleHBRUSH::Find(HBRUSH hBRUSH)
	{
		TinyHandleHBRUSH** ps = m_map.Find(hBRUSH);
		if (ps != NULL)
		{
			return *ps;
		}
		return NULL;
	}
	TinyHandleMap<HBRUSH, TinyHandleHBRUSH*> TinyHandleHBRUSH::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHPALETTE, TinyObject);
	TinyHandleHPALETTE::TinyHandleHPALETTE()
		:m_hPALETTE(NULL)
	{

	}
	TinyHandleHPALETTE::~TinyHandleHPALETTE()
	{

	}
	TinyHandleHPALETTE::operator HPALETTE() const
	{
		return this == NULL ? NULL : m_hPALETTE;
	}
	HPALETTE TinyHandleHPALETTE::Handle() const
	{
		return this == NULL ? NULL : m_hPALETTE;
	}
	BOOL TinyHandleHPALETTE::operator == (const TinyHandleHPALETTE& obj) const
	{
		return obj.m_hPALETTE == m_hPALETTE;
	}
	BOOL TinyHandleHPALETTE::operator != (const TinyHandleHPALETTE& obj) const
	{
		return obj.m_hPALETTE != m_hPALETTE;
	}
	BOOL TinyHandleHPALETTE::Attach(HPALETTE hPALETTE)
	{
		if (!hPALETTE)
			return FALSE;
		m_hPALETTE = hPALETTE;
		TinyHandleHPALETTE* ps = this;
		m_map.Add(hPALETTE, ps);
		return TRUE;
	}
	HPALETTE TinyHandleHPALETTE::Detach()
	{
		HPALETTE hPALETTE = m_hPALETTE;
		if (hPALETTE != NULL)
		{
			m_map.Remove(hPALETTE);
		}
		m_hPALETTE = NULL;
		return hPALETTE;
	}
	TinyHandleHPALETTE* TinyHandleHPALETTE::Find(HPALETTE hPALETTE)
	{
		TinyHandleHPALETTE** ps = m_map.Find(hPALETTE);
		if (ps != NULL)
		{
			return *ps;
		}
		return NULL;
	}
	TinyHandleMap<HPALETTE, TinyHandleHPALETTE*> TinyHandleHPALETTE::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHRGN, TinyObject);
	TinyHandleHRGN::TinyHandleHRGN()
		:m_hHRGN(NULL)
	{

	}
	TinyHandleHRGN::~TinyHandleHRGN()
	{

	}
	TinyHandleHRGN::operator HRGN() const
	{
		return this == NULL ? NULL : m_hHRGN;
	}
	HRGN TinyHandleHRGN::Handle() const
	{
		return this == NULL ? NULL : m_hHRGN;
	}
	BOOL TinyHandleHRGN::operator == (const TinyHandleHRGN& obj) const
	{
		return obj.m_hHRGN == m_hHRGN;
	}
	BOOL TinyHandleHRGN::operator != (const TinyHandleHRGN& obj) const
	{
		return obj.m_hHRGN != m_hHRGN;
	}
	BOOL TinyHandleHRGN::Attach(HRGN hHRGN)
	{
		if (!hHRGN)
			return FALSE;
		m_hHRGN = hHRGN;
		TinyHandleHRGN* ps = this;
		TinyHandleHRGN::m_map.Add(m_hHRGN, ps);
		return TRUE;
	}
	HRGN TinyHandleHRGN::Detach()
	{
		HRGN hHRGN = m_hHRGN;
		if (hHRGN != NULL)
		{
			TinyHandleHRGN::m_map.Remove(hHRGN);
		}
		m_hHRGN = NULL;
		return hHRGN;
	}
	TinyHandleHRGN* TinyHandleHRGN::Find(HRGN hRGN)
	{
		TinyHandleHRGN** ps = m_map.Find(hRGN);
		if (ps != NULL)
		{
			return *ps;
		}
		return NULL;
	}
	TinyHandleMap<HRGN, TinyHandleHRGN*> TinyHandleHRGN::m_map;
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_DYNAMIC(TinyHandleHIMAGELIST, TinyObject);
	TinyHandleHIMAGELIST::TinyHandleHIMAGELIST()
		:m_hIMAGELIST(NULL)
	{

	}
	TinyHandleHIMAGELIST::~TinyHandleHIMAGELIST()
	{

	}
	TinyHandleHIMAGELIST::operator HIMAGELIST() const
	{
		return this == NULL ? NULL : m_hIMAGELIST;
	}
	HIMAGELIST TinyHandleHIMAGELIST::Handle() const
	{
		return this == NULL ? NULL : m_hIMAGELIST;
	}
	BOOL TinyHandleHIMAGELIST::operator == (const TinyHandleHIMAGELIST& obj) const
	{
		return obj.m_hIMAGELIST == m_hIMAGELIST;
	}
	BOOL TinyHandleHIMAGELIST::operator != (const TinyHandleHIMAGELIST& obj) const
	{
		return obj.m_hIMAGELIST != m_hIMAGELIST;
	}
	BOOL TinyHandleHIMAGELIST::Attach(HIMAGELIST hIMAGELIST)
	{
		if (!hIMAGELIST)
			return FALSE;
		m_hIMAGELIST = hIMAGELIST;
		TinyHandleHIMAGELIST* ps = this;
		m_map.Add(m_hIMAGELIST, ps);
		return TRUE;
	}
	HIMAGELIST TinyHandleHIMAGELIST::Detach()
	{
		HIMAGELIST hIMAGELIST = m_hIMAGELIST;
		if (hIMAGELIST != NULL)
		{
			m_map.Remove(hIMAGELIST);
		}
		m_hIMAGELIST = NULL;
		return hIMAGELIST;
	}
	TinyHandleHIMAGELIST* TinyHandleHIMAGELIST::Find(HIMAGELIST hIMAGELIST)
	{
		TinyHandleHIMAGELIST** ps = m_map.Find(hIMAGELIST);
		if (ps != NULL)
		{
			return *ps;
		}
		return NULL;
	}
	TinyHandleMap<HIMAGELIST, TinyHandleHIMAGELIST*> TinyHandleHIMAGELIST::m_map;
}