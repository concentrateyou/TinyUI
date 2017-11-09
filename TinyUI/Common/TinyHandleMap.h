#pragma once
#include "TinyWindowMsg.h"
#include "TinyCollection.h"
#include "TinyCore.h"

namespace TinyUI
{
	/// <summary>
	/// ¾ä±úÓ³ÉäÀà
	/// </summary>
	template<class T = HANDLE, class V = void*>
	class TinyHandleMap
	{
		DISALLOW_COPY_AND_ASSIGN(TinyHandleMap)
	public:
		TinyHandleMap();
		~TinyHandleMap();
		V* Find(T& _key);
		V* Find(const T& _key);
		V* operator[](T& _key);
		V* operator[](const T& _key);
		void Add(const T& _key, const V& _value);
		void Remove(const T& _key);
		void RemoveAll();
		INT GetSize() const;
	private:
		TinyLock			m_lock;
		TinySimpleMap<T, V> m_handleMap;
	};
	template<class T, class V>
	TinyHandleMap<T, V>::TinyHandleMap()
	{

	}
	template<class T, class V>
	TinyHandleMap<T, V>::~TinyHandleMap()
	{
		this->RemoveAll();
	}
	template<class T, class V>
	V* TinyHandleMap<T, V>::Find(T& _key)
	{
		TinyAutoLock lock(m_lock);
		return m_handleMap.Find(_key);
	}
	template<class T, class V>
	V* TinyHandleMap<T, V>::Find(const T& _key)
	{
		TinyAutoLock lock(m_lock);
		return m_handleMap.Find(_key);
	}
	template<class T, class V>
	V* TinyHandleMap<T, V>::operator[](T& _key)
	{
		TinyAutoLock lock(m_lock);
		return m_handleMap.GetValue(_key);
	}
	template<class T, class V>
	V* TinyHandleMap<T, V>::operator[](const T& _key)
	{
		TinyAutoLock lock(m_lock);
		return m_handleMap.GetValue(_key);
	}
	template<class T, class V>
	void TinyHandleMap<T, V>::Add(const T& _key, const V& _value)
	{
		TinyAutoLock lock(m_lock);
		m_handleMap.Add(_key, _value);
	}
	template<class T, class V>
	void TinyHandleMap<T, V>::Remove(const T& _key)
	{
		TinyAutoLock lock(m_lock);
		m_handleMap.Remove(_key);
	}
	template<class T, class V>
	void TinyHandleMap<T, V>::RemoveAll()
	{
		TinyAutoLock lock(m_lock);
		m_handleMap.RemoveAll();
	}
	template<class T, class V>
	INT TinyHandleMap<T, V>::GetSize() const
	{
		TinyAutoLock lock(m_lock);
		return m_handleMap.GetSize();
	}
	/// <summary>
	/// ´°¿Ú¾ä±úÀà
	/// </summary>
	class TinyHandleHWND :public TinyWindowMsg, public TinyMessageFilter, public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHWND)
	public:
		TinyHandleHWND();
		virtual ~TinyHandleHWND();
		operator HWND() const;
		HWND Handle() const;
		BOOL operator==(const TinyHandleHWND& obj) const;
		BOOL operator!=(const TinyHandleHWND& obj) const;
		BOOL Attach(HWND hWND);
		HWND Detach();
		static TinyHandleHWND* Find(HWND hWND);
	private:
		static TinyHandleMap<HWND, TinyHandleHWND*> m_map;
	public:
		HWND m_hWND;
	};
	/// <summary>
	/// ²Ëµ¥¾ä±úÀà
	/// </summary>
	class TinyHandleHMENU : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHMENU)
	public:
		TinyHandleHMENU();
		virtual ~TinyHandleHMENU();
		operator HMENU() const;
		HMENU Handle() const;
		BOOL operator==(const TinyHandleHMENU& obj) const;
		BOOL operator!=(const TinyHandleHMENU& obj) const;
		BOOL Attach(HMENU hMENU);
		HMENU Detach();
		static TinyHandleHMENU* Find(HMENU hMENU);
	private:
		static TinyHandleMap<HMENU, TinyHandleHMENU*> m_map;
	public:
		HMENU m_hMENU;
	};
	/// <summary>
	/// DC¾ä±úÀà
	/// </summary>
	class TinyHandleHDC : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHDC)
	public:
		TinyHandleHDC();
		virtual ~TinyHandleHDC();
		operator HDC() const;
		HDC Handle() const;
		BOOL operator==(const TinyHandleHDC& obj) const;
		BOOL operator!=(const TinyHandleHDC& obj) const;
		BOOL Attach(HDC hDC);
		HDC Detach();
		static TinyHandleHDC* Find(HDC hDC);
	private:
		static TinyHandleMap<HDC, TinyHandleHDC*> m_map;
	public:
		HDC m_hDC;
	};
	/// <summary>
	/// BITMAP¾ä±úÀà
	/// </summary>
	class TinyHandleHBITMAP : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHBITMAP)
	public:
		TinyHandleHBITMAP();
		virtual ~TinyHandleHBITMAP();
		operator HBITMAP() const;
		HBITMAP Handle() const;
		BOOL operator==(const TinyHandleHBITMAP& obj) const;
		BOOL operator!=(const TinyHandleHBITMAP& obj) const;
		BOOL Attach(HBITMAP hBITMAP);
		HBITMAP Detach();
		static TinyHandleHBITMAP* Find(HBITMAP hBITMAP);
	private:
		static TinyHandleMap<HBITMAP, TinyHandleHBITMAP*> m_map;
	public:
		HBITMAP m_hBITMAP;
	};
	/// <summary>
	/// FONT¾ä±úÀà
	/// </summary>
	class TinyHandleHFONT : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHFONT)
	public:
		TinyHandleHFONT();
		virtual ~TinyHandleHFONT();
		operator HFONT() const;
		HFONT Handle() const;
		BOOL operator==(const TinyHandleHFONT& obj) const;
		BOOL operator!=(const TinyHandleHFONT& obj) const;
		BOOL Attach(HFONT hFONT);
		HFONT Detach();
		static TinyHandleHFONT* Find(HFONT hFONT);
	private:
		static TinyHandleMap<HFONT, TinyHandleHFONT*> m_map;
	public:
		HFONT m_hFONT;
	};
	/// <summary>
	/// PEN¾ä±úÀà
	/// </summary>
	class TinyHandleHPEN : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHPEN)
	public:
		TinyHandleHPEN();
		virtual ~TinyHandleHPEN();
		operator HPEN() const;
		HPEN Handle() const;
		BOOL operator==(const TinyHandleHPEN& obj) const;
		BOOL operator!=(const TinyHandleHPEN& obj) const;
		BOOL Attach(HPEN hPEN);
		HPEN Detach();
		static TinyHandleHPEN* Find(HPEN hPEN);
	private:
		static TinyHandleMap<HPEN, TinyHandleHPEN*> m_map;
	public:
		HPEN m_hPEN;
	};
	/// <summary>
	/// BRUSH¾ä±úÀà
	/// </summary>
	class TinyHandleHBRUSH : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHBRUSH)
	public:
		TinyHandleHBRUSH();
		virtual ~TinyHandleHBRUSH();
		operator HBRUSH() const;
		HBRUSH Handle() const;
		BOOL operator==(const TinyHandleHBRUSH& obj) const;
		BOOL operator!=(const TinyHandleHBRUSH& obj) const;
		BOOL Attach(HBRUSH hBRUSH);
		HBRUSH Detach();
		static TinyHandleHBRUSH* Find(HBRUSH hBRUSH);
	private:
		static TinyHandleMap<HBRUSH, TinyHandleHBRUSH*> m_map;
	public:
		HBRUSH m_hBRUSH;
	};
	/// <summary>
	/// PALETTE¾ä±úÀà
	/// </summary>
	class TinyHandleHPALETTE : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHPALETTE)
	public:
		TinyHandleHPALETTE();
		virtual ~TinyHandleHPALETTE();
		operator HPALETTE() const;
		HPALETTE Handle() const;
		BOOL operator==(const TinyHandleHPALETTE& obj) const;
		BOOL operator!=(const TinyHandleHPALETTE& obj) const;
		BOOL Attach(HPALETTE hPALETTE);
		HPALETTE Detach();
		static TinyHandleHPALETTE* Find(HPALETTE hPALETTE);
	private:
		static TinyHandleMap<HPALETTE, TinyHandleHPALETTE*> m_map;
	public:
		HPALETTE m_hPALETTE;
	};
	/// <summary>
	/// RGN¾ä±úÀà
	/// </summary>
	class TinyHandleHRGN : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHRGN)
	public:
		TinyHandleHRGN();
		virtual ~TinyHandleHRGN();
		operator HRGN() const;
		HRGN Handle() const;
		BOOL operator==(const TinyHandleHRGN& obj) const;
		BOOL operator!=(const TinyHandleHRGN& obj) const;
		BOOL Attach(HRGN hHRGN);
		HRGN Detach();
		static TinyHandleHRGN* Find(HRGN hHRGN);
	private:
		static TinyHandleMap<HRGN, TinyHandleHRGN*> m_map;
	public:
		HRGN m_hHRGN;
	};
	/// <summary>
	/// IMAGELIST¾ä±úÀà
	/// </summary>
	class TinyHandleHIMAGELIST : public TinyObject
	{
		DECLARE_DYNAMIC(TinyHandleHIMAGELIST)
	public:
		TinyHandleHIMAGELIST();
		virtual ~TinyHandleHIMAGELIST();
		operator HIMAGELIST() const;
		HIMAGELIST Handle() const;
		BOOL operator==(const TinyHandleHIMAGELIST& obj) const;
		BOOL operator!=(const TinyHandleHIMAGELIST& obj) const;
		BOOL Attach(HIMAGELIST hIMAGELIST);
		HIMAGELIST Detach();
		static TinyHandleHIMAGELIST* Find(HIMAGELIST hIMAGELIST);
	private:
		static TinyHandleMap<HIMAGELIST, TinyHandleHIMAGELIST*> m_map;
	public:
		HIMAGELIST m_hIMAGELIST;
	};
}

