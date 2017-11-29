#include "../stdafx.h"
#include "TinyCommon.h"

namespace TinyUI
{
	void __cdecl Trace(LPCTSTR lpszFormat, ...)
	{
		TCHAR szBuffer[4096];
		va_list args;
		va_start(args, lpszFormat);
		_vstprintf_s(szBuffer, _countof(szBuffer), lpszFormat, args);
		va_end(args);
		::OutputDebugString(szBuffer);
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
