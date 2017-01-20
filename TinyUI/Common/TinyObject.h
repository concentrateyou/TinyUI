#pragma once
#include "TinyCommon.h"

namespace TinyUI
{
	class NO_VTABLE TinyObject;
	struct TinyRuntimeClass
	{
		LPCSTR	m_pszClassName;
		DWORD	m_dwObjectSize;
		TinyRuntimeClass* m_pBaseClass;
		TinyRuntimeClass* m_pNextClass;
	};

#define RUNTIME_CLASS(class_name) ((TinyRuntimeClass*)(&class_name::class##class_name))

#define DECLARE_DYNAMIC(class_name) \
public: \
	static const TinyRuntimeClass class##class_name; \
	virtual TinyRuntimeClass* GetRuntimeClass() const; 

#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	SELECTANY const TinyRuntimeClass class_name::class##class_name = { #class_name, sizeof(class class_name), RUNTIME_CLASS(base_class_name), NULL }; \
	TinyRuntimeClass* class_name::GetRuntimeClass() const { return RUNTIME_CLASS(class_name); }

	/// <summary>
	/// 对象基类
	/// </summary>
	class NO_VTABLE TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinyObject)
	public:
		TinyObject();
	public:
		virtual ~TinyObject();
		BOOL IsKindof(const TinyRuntimeClass* pClass) const;
		BOOL IsKindof(const TinyObject* pObject) const;
		virtual TinyRuntimeClass* GetRuntimeClass() const;
	public:
		static const TinyRuntimeClass classTinyObject;
	};
};

