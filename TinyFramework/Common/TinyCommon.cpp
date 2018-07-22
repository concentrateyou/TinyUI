#include "../stdafx.h"
#include "TinyCommon.h"

namespace TinyFramework
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
	const VARIANT ScopedVariant::EmptyVariant = { { { VT_EMPTY } } };
	ScopedVariant::ScopedVariant()
	{
		m_var.vt = VT_EMPTY;
		VariantInit(&m_var);
	}
	ScopedVariant::ScopedVariant(const WCHAR* str)
	{
		m_var.vt = VT_EMPTY;
		Set(str);
	}

	ScopedVariant::ScopedVariant(const WCHAR* str, UINT length)
	{
		m_var.vt = VT_BSTR;
		m_var.bstrVal = ::SysAllocStringLen(str, length);
	}

	ScopedVariant::ScopedVariant(INT value, VARTYPE vt)
	{
		m_var.vt = vt;
		m_var.lVal = value;
	}

	ScopedVariant::ScopedVariant(DOUBLE value, VARTYPE vt)
	{
		m_var.vt = vt;
		m_var.dblVal = value;
	}

	ScopedVariant::ScopedVariant(IDispatch* dispatch)
	{
		m_var.vt = VT_EMPTY;
		Set(dispatch);
	}

	ScopedVariant::ScopedVariant(IUnknown* unknown)
	{
		m_var.vt = VT_EMPTY;
		Set(unknown);
	}

	ScopedVariant::ScopedVariant(SAFEARRAY* safearray)
	{
		m_var.vt = VT_EMPTY;
		Set(safearray);
	}

	ScopedVariant::ScopedVariant(const VARIANT& var)
	{
		m_var.vt = VT_EMPTY;
		Set(var);
	}
	ScopedVariant::~ScopedVariant()
	{
		VariantClear(&m_var);
	}
	void ScopedVariant::Reset()
	{
		VariantClear(&m_var);
	}
	void ScopedVariant::Reset(const VARIANT& var)
	{
		if (&var != &m_var)
		{
			::VariantClear(&m_var);
			m_var = var;
		}
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
	void ScopedVariant::Set(const WCHAR* str)
	{
		m_var.vt = VT_BSTR;
		m_var.bstrVal = ::SysAllocString(str);
	}

	void ScopedVariant::Set(CHAR i8)
	{
		m_var.vt = VT_I1;
		m_var.cVal = i8;
	}

	void ScopedVariant::Set(BYTE ui8)
	{
		m_var.vt = VT_UI1;
		m_var.bVal = ui8;
	}

	void ScopedVariant::Set(INT16 i16)
	{
		m_var.vt = VT_I2;
		m_var.iVal = i16;
	}

	void ScopedVariant::Set(UINT16 ui16)
	{
		m_var.vt = VT_UI2;
		m_var.uiVal = ui16;
	}

	void ScopedVariant::Set(INT32 i32)
	{
		m_var.vt = VT_I4;
		m_var.lVal = i32;
	}

	void ScopedVariant::Set(UINT32 ui32)
	{
		m_var.vt = VT_UI4;
		m_var.ulVal = ui32;
	}

	void ScopedVariant::Set(INT64 i64)
	{
		m_var.vt = VT_I8;
		m_var.llVal = i64;
	}

	void ScopedVariant::Set(UINT64 ui64)
	{
		m_var.vt = VT_UI8;
		m_var.ullVal = ui64;
	}

	void ScopedVariant::Set(FLOAT r32)
	{
		m_var.vt = VT_R4;
		m_var.fltVal = r32;
	}

	void ScopedVariant::Set(DOUBLE r64)
	{
		m_var.vt = VT_R8;
		m_var.dblVal = r64;
	}

	void ScopedVariant::SetDate(DATE date)
	{
		m_var.vt = VT_DATE;
		m_var.date = date;
	}

	void ScopedVariant::Set(IDispatch* disp)
	{
		m_var.vt = VT_DISPATCH;
		m_var.pdispVal = disp;
		if (disp)
			disp->AddRef();
	}

	void ScopedVariant::Set(bool b)
	{
		m_var.vt = VT_BOOL;
		m_var.boolVal = b ? VARIANT_TRUE : VARIANT_FALSE;
	}

	void ScopedVariant::Set(IUnknown* unk)
	{
		m_var.vt = VT_UNKNOWN;
		m_var.punkVal = unk;
		if (unk)
			unk->AddRef();
	}

	void ScopedVariant::Set(SAFEARRAY* array)
	{
		if (SUCCEEDED(::SafeArrayGetVartype(array, &m_var.vt)))
		{
			m_var.vt |= VT_ARRAY;
			m_var.parray = array;
		}
		else 
		{
			m_var.vt = VT_EMPTY;
		}
	}

	void ScopedVariant::Set(const VARIANT& var)
	{
		if (FAILED(::VariantCopy(&m_var, &var)))
		{
			m_var.vt = VT_EMPTY;
		}
	}

	ScopedVariant& ScopedVariant::operator=(const VARIANT& var) 
	{
		if (&var != &m_var) 
		{
			VariantClear(&m_var);
			Set(var);
		}
		return *this;
	}
};
