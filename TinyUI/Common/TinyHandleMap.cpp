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
		m_map.Add(static_cast<LPVOID>(m_hWND), static_cast<LPVOID>(this));
		return TRUE;
	}
	HWND TinyHandleHWND::Detach()
	{
		HWND hWND = m_hWND;
		if (hWND != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(m_hWND));
		}
		m_hWND = NULL;
		return hWND;
	}
	TinyHandleHWND* TinyHandleHWND::Lookup(HWND hWND)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hWND), val))
			return NULL;
		return reinterpret_cast<TinyHandleHWND*>(val);
	}
	TinyPointerMap TinyHandleHWND::m_map;
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
		m_map.Add(static_cast<LPVOID>(hMENU), static_cast<LPVOID>(this));
		return TRUE;
	}
	HMENU TinyHandleHMENU::Detach()
	{
		HMENU hMENU = m_hMENU;
		if (hMENU != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(m_hMENU));
		}
		m_hMENU = NULL;
		return hMENU;
	}
	TinyHandleHMENU* TinyHandleHMENU::Lookup(HMENU hMENU)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hMENU), val))
			return NULL;
		return reinterpret_cast<TinyHandleHMENU*>(val);
	}
	TinyPointerMap TinyHandleHMENU::m_map;
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
		m_map.Add(static_cast<LPVOID>(hDC), static_cast<LPVOID>(this));
		return TRUE;
	}
	HDC TinyHandleHDC::Detach()
	{
		HDC hDC = m_hDC;
		if (hDC != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(m_hDC));
		}
		m_hDC = NULL;
		return hDC;
	}
	TinyHandleHDC* TinyHandleHDC::Lookup(HDC hDC)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hDC), val))
			return NULL;
		return reinterpret_cast<TinyHandleHDC*>(val);
	}
	TinyPointerMap TinyHandleHDC::m_map;
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
		m_map.Add(static_cast<LPVOID>(hBITMAP), static_cast<LPVOID>(this));
		return TRUE;
	}
	HBITMAP TinyHandleHBITMAP::Detach()
	{
		HBITMAP hBITMAP = m_hBITMAP;
		if (hBITMAP != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(m_hBITMAP));
		}
		m_hBITMAP = NULL;
		return hBITMAP;
	}
	TinyHandleHBITMAP* TinyHandleHBITMAP::Lookup(HBITMAP hBITMAP)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hBITMAP), val))
			return NULL;
		return reinterpret_cast<TinyHandleHBITMAP*>(val);
	}
	TinyPointerMap TinyHandleHBITMAP::m_map;
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
		m_map.Add(static_cast<LPVOID>(hFONT), static_cast<LPVOID>(this));
		return TRUE;
	}
	HFONT TinyHandleHFONT::Detach()
	{
		HFONT hFONT = m_hFONT;
		if (hFONT != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(m_hFONT));
		}
		m_hFONT = NULL;
		return hFONT;
	}
	TinyHandleHFONT* TinyHandleHFONT::Lookup(HFONT hFONT)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hFONT), val))
			return NULL;
		return reinterpret_cast<TinyHandleHFONT*>(val);
	}
	TinyPointerMap TinyHandleHFONT::m_map;
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
		m_map.Add(static_cast<LPVOID>(hPEN), static_cast<LPVOID>(this));
		return TRUE;
	}
	HPEN TinyHandleHPEN::Detach()
	{
		HPEN hPEN = m_hPEN;
		if (hPEN != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(m_hPEN));
		}
		m_hPEN = NULL;
		return hPEN;
	}
	TinyHandleHPEN* TinyHandleHPEN::Lookup(HPEN hPEN)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hPEN), val))
			return NULL;
		return reinterpret_cast<TinyHandleHPEN*>(val);
	}
	TinyPointerMap TinyHandleHPEN::m_map;
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
		m_map.Add(static_cast<LPVOID>(hBRUSH), static_cast<LPVOID>(this));
		return TRUE;
	}
	HBRUSH TinyHandleHBRUSH::Detach()
	{
		HBRUSH hBRUSH = m_hBRUSH;
		if (hBRUSH != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(hBRUSH));
		}
		m_hBRUSH = NULL;
		return hBRUSH;
	}
	TinyHandleHBRUSH* TinyHandleHBRUSH::Lookup(HBRUSH hBRUSH)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hBRUSH), val))
			return NULL;
		return reinterpret_cast<TinyHandleHBRUSH*>(val);
	}
	TinyPointerMap TinyHandleHBRUSH::m_map;
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
		m_map.Add(static_cast<LPVOID>(hPALETTE), static_cast<LPVOID>(this));
		return TRUE;
	}
	HPALETTE TinyHandleHPALETTE::Detach()
	{
		HPALETTE hPALETTE = m_hPALETTE;
		if (hPALETTE != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(hPALETTE));
		}
		m_hPALETTE = NULL;
		return hPALETTE;
	}
	TinyHandleHPALETTE* TinyHandleHPALETTE::Lookup(HPALETTE hPALETTE)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hPALETTE), val))
			return NULL;
		return reinterpret_cast<TinyHandleHPALETTE*>(val);
	}
	TinyPointerMap TinyHandleHPALETTE::m_map;
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
		m_map.Add(static_cast<LPVOID>(hHRGN), static_cast<LPVOID>(this));
		return TRUE;
	}
	HRGN TinyHandleHRGN::Detach()
	{
		HRGN hHRGN = m_hHRGN;
		if (hHRGN != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(hHRGN));
		}
		m_hHRGN = NULL;
		return hHRGN;
	}
	TinyHandleHRGN* TinyHandleHRGN::Lookup(HRGN hRGN)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hRGN), val))
			return NULL;
		return reinterpret_cast<TinyHandleHRGN*>(val);
	}
	TinyPointerMap TinyHandleHRGN::m_map;
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
		m_map.Add(static_cast<LPVOID>(hIMAGELIST), static_cast<LPVOID>(this));
		return TRUE;
	}
	HIMAGELIST TinyHandleHIMAGELIST::Detach()
	{
		HIMAGELIST hIMAGELIST = m_hIMAGELIST;
		if (hIMAGELIST != NULL)
		{
			m_map.Remove(static_cast<LPVOID>(hIMAGELIST));
		}
		m_hIMAGELIST = NULL;
		return hIMAGELIST;
	}
	TinyHandleHIMAGELIST* TinyHandleHIMAGELIST::Lookup(HIMAGELIST hIMAGELIST)
	{
		LPVOID val = 0;
		if (!m_map.Lookup(static_cast<LPVOID>(hIMAGELIST), val))
			return NULL;
		return reinterpret_cast<TinyHandleHIMAGELIST*>(val);
	}
	TinyPointerMap TinyHandleHIMAGELIST::m_map;
}