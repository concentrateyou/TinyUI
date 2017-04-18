#include "../stdafx.h"
#include "TinyObject.h"

namespace TinyUI
{
	TinyRuntimeClass* TinyRuntimeClass::m_pFirstClass = NULL;

	TinyRuntimeClass* PASCAL TinyRuntimeClass::FromName(LPCSTR lpszClassName)
	{
		const TinyRuntimeClass* pClass = &TinyObject::classTinyObject;
		while (pClass != NULL)
		{
			TRACE("className:%s\n", pClass->m_pszClassName);
			if (lstrcmpA(lpszClassName, pClass->m_pszClassName) == 0)
			{
				return const_cast<TinyRuntimeClass*>(pClass);
			}
			pClass = pClass->m_pNextClass;
		}
		return NULL;
	}
	CLASSINIT::CLASSINIT(const TinyRuntimeClass* pNewClass)
	{
		TinyRuntimeClass::m_pFirstClass = const_cast<TinyRuntimeClass*>(pNewClass);
	}
	//////////////////////////////////////////////////////////////////////////
	const TinyRuntimeClass TinyObject::classTinyObject = { "TinyObject", sizeof(TinyObject), NULL, NULL, (TinyRuntimeClass*)TinyRuntimeClass::m_pFirstClass };

	TinyObject::TinyObject()
	{
	}

	TinyObject::~TinyObject()
	{

	}

	BOOL TinyObject::IsKindOf(const TinyRuntimeClass* pClass) const
	{
		TinyRuntimeClass* pClassThis = this->GetRuntimeClass();
		while (pClassThis != NULL)
		{
			if (pClassThis == pClass)
				return TRUE;
			pClassThis = pClassThis->m_pBaseClass;
		}
		return FALSE;
	}
	BOOL TinyObject::IsKindOf(const TinyObject* pObject) const
	{
		TinyRuntimeClass* pClassThis = this->GetRuntimeClass();
		while (pClassThis != NULL)
		{
			if (pClassThis == pObject->GetRuntimeClass())
				return TRUE;
			pClassThis = pClassThis->m_pBaseClass;
		}
		return FALSE;
	}
	TinyRuntimeClass* TinyObject::GetRuntimeClass() const
	{
		return ((TinyRuntimeClass*)(&TinyObject::classTinyObject));
	}

	TinyObject* PASCAL TinyObject::FromName(LPCSTR lpszClassName)
	{
		TinyRuntimeClass* pClass = TinyRuntimeClass::FromName(lpszClassName);
		if (pClass && pClass->m_pNew)
		{
			return pClass->m_pNew();
		}
		return NULL;
	}
};
