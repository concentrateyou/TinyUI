#include "../stdafx.h"
#include "TinyObject.h"

namespace TinyFramework
{
	TinyRuntimeClass* TinyRuntimeClass::pFirstClass = NULL;

	TinyRuntimeClass* PASCAL TinyRuntimeClass::FromName(LPCSTR lpszClassName)
	{
		TinyRuntimeClass* pClass = TinyRuntimeClass::pFirstClass;
		while (pClass != NULL)
		{
			if (lstrcmpA(lpszClassName, pClass->m_lpszClassName) == 0)
			{
				return const_cast<TinyRuntimeClass*>(pClass);
			}
			pClass = pClass->m_pNextClass;
		}
		return NULL;
	}

	CLASSINIT::CLASSINIT(const TinyRuntimeClass* pNewClass)
	{
		TinyRuntimeClass::pFirstClass = const_cast<TinyRuntimeClass*>(pNewClass);
	}
	//////////////////////////////////////////////////////////////////////////
	SELECTANY const TinyRuntimeClass TinyObject::classTinyObject = { TEXT("TinyObject"), sizeof(class TinyObject), NULL, NULL, (TinyRuntimeClass*)TinyRuntimeClass::pFirstClass };

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
