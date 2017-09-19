#include "../stdafx.h"
#include "TinyObject.h"

namespace TinyUI
{
	GlobalRuntime::GlobalRuntime()
	{
		
	}
	GlobalRuntime::~GlobalRuntime()
	{

	}
	STATIC_DATA GlobalRuntime g_runtime;
	//////////////////////////////////////////////////////////////////////////
	TinyRuntimeClass* PASCAL TinyRuntimeClass::FromName(LPCSTR lpszClassName)
	{
		if (!lpszClassName)
			return NULL;
		TinyRuntimeClass* pClass = NULL;
		for (pClass = g_runtime.m_classList; pClass != NULL;pClass = pClass->m_pNextClass)
		{
			if (lstrcmpA(lpszClassName, pClass->m_pszClassName) == 0)
			{
				return pClass;
			}
		}
		return NULL;
	}

	CLASSINIT::CLASSINIT(register TinyRuntimeClass* pNewClass)
	{
		g_runtime.m_classList.Add(pNewClass);
	}
	//////////////////////////////////////////////////////////////////////////
	const TinyRuntimeClass TinyObject::classTinyObject = { TEXT("TinyObject"), sizeof(class TinyObject), NULL, NULL, NULL };

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
