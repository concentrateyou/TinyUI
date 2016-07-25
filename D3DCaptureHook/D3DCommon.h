#pragma once
#include <Windows.h>
#include <Shellapi.h>
#include <Commdlg.h>
#include <comdef.h>
#include <Mmsystem.h>
#include <new.h>
#include <WinGDI.h>
#include <GdiPlus.h>
#include <Shlwapi.h> 
#include <commctrl.inl>
#include <Psapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>
#include <crtdbg.h>
#include <utility>
#include <algorithm>
#include <limits>
#include <string>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shlwapi.lib")
using namespace std;

#ifndef ASSERT
#define ASSERT(expr) _ASSERTE(expr)
#endif 
void Trace(LPCTSTR lpszFormat, ...);
#ifndef TRACE
#define TRACE Trace
#endif
#ifndef ASSUME
#define ASSUME(expr) do { ASSERT(expr); __analysis_assume(!!(expr)); } while(0)
#endif

#define IS_VALID_STRING_PTR(ptr, type) \
	(!IsBadStringPtr((ptr), (UINT)-1))

#define DISALLOW_COPY(TypeName) \
private:\
	TypeName(const TypeName&) = delete;

#define DISALLOW_ASSIGN(TypeName) \
private:\
	void operator=(const TypeName&) = delete;

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
private:\
	TypeName(const TypeName&) = delete;  \
	void operator=(const TypeName&) = delete;

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
private:\
	TypeName() = delete;   \
	DISALLOW_COPY_AND_ASSIGN(TypeName);

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  { if (p) { delete (p);  (p)=NULL; } }
#endif    

template<typename T, size_t N>
char(&ArraySizeHelper(T(&array)[N]))[N];
#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#define ARRAYSIZE_UNSAFE(a) \
	((sizeof(a) / sizeof(*(a))) / \
	static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

/// <summary>
/// �߾��ȼ�����
/// </summary>
class CPerformanceTimer
{
	DISALLOW_COPY_AND_ASSIGN(CPerformanceTimer);
public:
	CPerformanceTimer();
	~CPerformanceTimer();
	LONGLONG GetMicroseconds();
	void BeginTime();
	void EndTime();
private:
	BOOL			m_bRunning;
	DWORD			m_dwTimerMask;
	DWORD			m_dwTick;
	LONGLONG		m_lastTime;
	LARGE_INTEGER	m_lFrequency;
	LARGE_INTEGER	m_startPerformanceCount;
};

class CScopedLibrary
{
	DISALLOW_COPY_AND_ASSIGN(CScopedLibrary);
public:
	CScopedLibrary(const string& str);
	~CScopedLibrary();
	operator HINSTANCE() const;
	HINSTANCE Handle() const;
private:
	HINSTANCE	m_hInstance;
};

template <class T>
class CScopedArray
{
	DISALLOW_COPY_AND_ASSIGN(CScopedArray);
public:
	explicit CScopedArray(T* ps = 0);
	~CScopedArray();
	void Reset(T* ps = 0) throw();
	T& operator[](INT i) const;
	BOOL operator==(T* ps) const;
	BOOL operator!=(T* ps) const;
	T* Ptr() const throw();
public:
	T* _myP;
};
template<class T>
CScopedArray<T>::CScopedArray(T* ps)
	: _myP(ps)
{

}
template<class T>
CScopedArray<T>::~CScopedArray()
{
	SAFE_DELETE_ARRAY(_myP);
}
template<class T>
T& CScopedArray<T>::operator[](INT i) const
{
	ASSERT(i >= 0);
	ASSERT(_myP != NULL);
	return _myP[i];
}
template<class T>
BOOL CScopedArray<T>::operator==(T* ps) const
{
	return _myP == ps;
}
template<class T>
BOOL CScopedArray<T>::operator!=(T* ps) const
{
	return _myP != ps;
}
template<class T>
void CScopedArray<T>::Reset(T* ps) throw()
{
	if (ps != _myP)
		delete[] _myP;
	_myP = ps;
}
template<class T>
T* CScopedArray<T>::Ptr() const throw()
{
	return _myP;
}
/// <summary>
/// COM����ָ��
/// </summary>
template <class T>
class CComPtr
{
public:
	T* _myP;
public:
	CComPtr(T* lp = NULL);
	CComPtr(const CComPtr<T>& lp);
	~CComPtr();
	void Release();
	operator T*() const;
	T*	Ptr() const;
	T& operator*() const;
	T** operator&();
	T* operator->();
	T* operator=(T* lp);
	T* operator=(const CComPtr<T>& lp);
	BOOL operator!();
private:
	static IUnknown*  ComPtrAssign(IUnknown** pp, IUnknown* lp);
};
template<class T>
CComPtr<T>::CComPtr(T* lp)
{
	if ((_myP = lp) != NULL)
		_myP->AddRef();
}
template<class T>
CComPtr<T>::CComPtr(const CComPtr<T>& lp)
{
	if ((_myP = lp._myP) != NULL)
		_myP->AddRef();
}
template<class T>
CComPtr<T>::~CComPtr()
{
	if (_myP)
		_myP->Release();
}
template<class T>
void CComPtr<T>::Release()
{
	if (_myP)
		_myP->Release();
	_myP = NULL;
}
template<class T>
CComPtr<T>::operator T*() const
{
	return (T*)_myP;
}
template<class T>
T*	CComPtr<T>::Ptr() const
{
	return (T*)_myP;
}
template<class T>
T& CComPtr<T>::operator*() const
{
	ASSERT(_myP != NULL);
	return *_myP;
}
template<class T>
T** CComPtr<T>::operator&()
{
	ASSERT(_myP == NULL);
	return &_myP;
}
template<class T>
T* CComPtr<T>::operator->()
{
	ASSERT(_myP != NULL);
	return _myP;
}
template<class T>
T* CComPtr<T>::operator=(T* lp)
{
	return (T*)ComPtrAssign((IUnknown**)&_myP, lp);
}
template<class T>
T* CComPtr<T>::operator=(const CComPtr<T>& lp)
{
	return (T*)ComPtrAssign((IUnknown**)&_myP, lp.p);
}
template<class T>
BOOL CComPtr<T>::operator!()
{
	return (_myP == NULL) ? TRUE : FALSE;
}
template<class T>
IUnknown*  CComPtr<T>::ComPtrAssign(IUnknown** pp, IUnknown* lp)
{
	if (lp != NULL)
		lp->AddRef();
	if (*pp)
		(*pp)->Release();
	*pp = lp;
	return lp;
}