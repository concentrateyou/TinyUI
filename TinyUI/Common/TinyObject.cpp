#include "../stdafx.h"
#include "TinyObject.h"

namespace TinyUI
{
	TinyRuntimeClass* TinyRuntimeClass::pFirstClass = NULL;
	TinyRuntimeClass TinyObject::classTinyObject = { "TinyObject", sizeof(TinyObject),NULL, NULL };
	static CLASSINIT _init_TinyObject(RUNTIME_CLASS(TinyObject));

	TinyObject::TinyObject()
	{
	}

	TinyObject::~TinyObject()
	{

	}

	BOOL TinyObject::IsKindof(const TinyRuntimeClass* pClass) const
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
	BOOL TinyObject::IsKindof(const TinyObject* pObject) const
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
};
