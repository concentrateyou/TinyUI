#pragma once
#include "TinyCommon.h"
#include "TinyCollection.h"

namespace TinyFramework
{
	class NO_VTABLE TinyObject;
	struct TinyRuntimeClass;

	struct CLASSINIT
	{
		CLASSINIT(const TinyRuntimeClass* pNewClass);
	};
	/// <summary>
	/// https://stackoverflow.com/questions/23797681/c-static-variable-in-lib-does-not-initialize
	/// 必须启用库依赖项输入
	/// </summary>
	struct TinyRuntimeClass
	{
		LPCSTR	m_lpszClassName;
		INT		m_objectSize;
		TinyObject* (PASCAL* m_pNew)();
		TinyRuntimeClass* m_pBaseClass;
		TinyRuntimeClass* m_pNextClass;
		static TinyRuntimeClass* pFirstClass;
		static TinyRuntimeClass* PASCAL FromName(LPCSTR lpszClassName);
	};

#define CLASS_NAME(class_name) (#class_name)

#define RUNTIME_CLASS(class_name) ((TinyRuntimeClass*)(&class_name::class##class_name))

#define DECLARE_DYNAMIC(class_name) \
public: \
	static const TinyRuntimeClass class##class_name; \
	virtual TinyRuntimeClass* GetRuntimeClass() const; 

#define DECLARE_DYNCREATE(class_name) \
	DECLARE_DYNAMIC(class_name) \
	static TinyObject* PASCAL New();

#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
	SELECTANY const TinyRuntimeClass class_name::class##class_name = { #class_name, sizeof(class class_name), NULL, RUNTIME_CLASS(base_class_name), TinyRuntimeClass::pFirstClass }; \
	TinyRuntimeClass* class_name::GetRuntimeClass() const { return RUNTIME_CLASS(class_name); } \
	static CLASSINIT _init_##class_name(&class_name::class##class_name); 


#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
	TinyObject* PASCAL class_name::New() { return new class_name; } \
	SELECTANY const TinyRuntimeClass class_name::class##class_name = { #class_name, sizeof(class class_name), class_name::New, RUNTIME_CLASS(base_class_name), TinyRuntimeClass::pFirstClass }; \
	TinyRuntimeClass* class_name::GetRuntimeClass() const { return RUNTIME_CLASS(class_name); } \
	static CLASSINIT _init_##class_name(&class_name::class##class_name); 

	/// <summary>
	/// 对象基类
	/// </summary>
	class NO_VTABLE TinyObject
	{
		DISALLOW_COPY_AND_ASSIGN(TinyObject)
	protected:
		TinyObject();
	public:
		virtual ~TinyObject() = 0;
		BOOL IsKindOf(const TinyRuntimeClass* pClass) const;
		BOOL IsKindOf(const TinyObject* pObject) const;
		virtual TinyRuntimeClass* GetRuntimeClass() const;
	public:
		static TinyObject* PASCAL FromName(LPCSTR lpszClassName);
	public:
		static const TinyRuntimeClass classTinyObject;
	};
};

