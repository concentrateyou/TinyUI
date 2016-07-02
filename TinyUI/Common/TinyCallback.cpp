#include "../stdafx.h"
#include "TinyObject.h"
#include "TinyCallback.h"

namespace TinyUI
{
	BOOL CallbackBase::IsNull() const
	{
		return m_storage.Ptr() == NULL;
	}
	void CallbackBase::Reset()
	{
		m_storage = NULL;
		m_invoke = NULL;
	}
	BOOL CallbackBase::operator==(const CallbackBase& other) const
	{
		return m_storage.Ptr() == other.m_storage.Ptr() && m_invoke == other.m_invoke;
	}
	CallbackBase::CallbackBase(InvokeFunctionBase invoke, InvokerStorageBase* storage)
		: m_invoke(invoke),
		m_storage(storage)
	{

	}

	CallbackBase::CallbackBase(CallbackBase&& other)
		:m_storage(std::move(other.m_storage)),
		m_invoke(other.m_invoke)
	{
		other.m_invoke = NULL;
	}

	CallbackBase::CallbackBase(const CallbackBase& other)
		: m_invoke(other.m_invoke),
		m_storage(other.m_storage)
	{
	}
	CallbackBase& CallbackBase::operator = (CallbackBase&& other)
	{
		m_storage = std::move(other.m_storage);
		m_invoke = other.m_invoke;
		other.m_invoke = nullptr;
		return *this;
	}
	CallbackBase& CallbackBase::operator = (const CallbackBase& other)
	{
		if (&other != this)
		{
			m_invoke = other.m_invoke;
			m_storage = other.m_storage;
		}
		return *this;
	}
	CallbackBase::~CallbackBase()
	{
	}
};

