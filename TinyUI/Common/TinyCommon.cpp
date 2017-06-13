#include "../stdafx.h"
#include "TinyCommon.h"

namespace TinyUI
{
	void __cdecl Trace(LPCTSTR lpszFormat, ...)
	{
		TCHAR szBuffer[512];
		va_list args;
		va_start(args, lpszFormat);
		INT iRes = _vstprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args);
		ASSERT(iRes >= 0);
		OutputDebugString(szBuffer);
		va_end(args);
	}
	//////////////////////////////////////////////////////////////////////////
	INLINE BOOL operator==(const NullType&, const NullType&)
	{
		return TRUE;
	}
	INLINE BOOL operator<(const NullType&, const NullType&)
	{
		return FALSE;
	}
	INLINE BOOL operator>(const NullType&, const NullType&)
	{
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	TinyReferenceBase::TinyReferenceBase()
		:m_cRef(0)
	{

	}
	TinyReferenceBase::~TinyReferenceBase()
	{

	}
	void TinyReferenceBase::AddRef() const
	{
		InterlockedIncrement(&m_cRef);
	}
	BOOL TinyReferenceBase::Release() const
	{
		return !InterlockedDecrement(&m_cRef);
	}
	LONG TinyReferenceBase::GetReference() const
	{
		return this->m_cRef;
	}
	//////////////////////////////////////////////////////////////////////////
	ScopedVariant::ScopedVariant()
	{
		VariantInit(&m_var);
	}
	ScopedVariant::~ScopedVariant()
	{
		VariantClear(&m_var);
	}
	void ScopedVariant::Reset()
	{
		VariantClear(&m_var);
	}
	ScopedVariant::operator const VARIANT&() const throw()
	{
		return m_var;
	}
	VARIANT* ScopedVariant::operator->()
	{
		return &m_var;
	}
	VARIANT* ScopedVariant::operator&()
	{
		return &m_var;
	}
};
