#pragma once
#include "TinyCommon.h"
#include <typeinfo>

namespace TinyUI
{
	template <typename Functor>
	class RunnableAdapter;
	template <typename R, typename... Args>
	class RunnableAdapter < R(*)(Args...) >
	{
	public:
		typedef R ReturnType;
		typedef R(*RunType)(Args...);
	};
	template <typename R, typename T, typename... Args>
	class RunnableAdapter < R(T::*)(Args...) >
	{
	public:
		typedef R ReturnType;
		typedef R(T::*RunType)(Args...);
	};
	template <typename R, typename T, typename... Args>
	class RunnableAdapter < R(T::*)(Args...) const >
	{
	public:
		typedef R ReturnType;
		typedef R(T::*RunType)(Args...) const;
	};
	template <typename T>
	struct FunctorTraits
	{
		using RunnableType = RunnableAdapter < T >;
		using RunType = typename RunnableType::RunType;
	};
	template <typename T>
	typename FunctorTraits<T>::RunnableType MakeRunnable(const T& myT)
	{
		return RunnableAdapter<T>(myT);
	}
	//////////////////////////////////////////////////////////////////////////
	class InvokerStorageBase : public TinyReference < InvokerStorageBase >
	{
	public:
		virtual ~InvokerStorageBase() {}
	};
	//////////////////////////////////////////////////////////////////////////
	template <typename InvokerStorage, typename RunType>
	struct Invoker;
	template <typename InvokerStorage, typename R, typename... Args>
	struct Invoker < InvokerStorage, R(*)(Args...) >
	{
		typedef R(*DoInvokeType)(InvokerStorageBase*, Args...);

		static R DoInvoke(InvokerStorageBase* base, Args... args)
		{
			InvokerStorage* call = static_cast<InvokerStorage*>(base);
			return (*call->m_runnable)(std::forward<Args>(args)...);
		}
	};
	template <typename InvokerStorage, typename R, typename T, typename... Args>
	struct Invoker < InvokerStorage, R(T::*)(Args...) >
	{
		typedef R(*DoInvokeType)(InvokerStorageBase*, Args...);

		static R DoInvoke(InvokerStorageBase* base, Args... args)
		{
			InvokerStorage* call = static_cast<InvokerStorage*>(base);
			return ((call->m_instance)->*call->m_runnable)(std::forward<Args>(args)...);
		}
	};
	template <typename InvokerStorage, typename R, typename T, typename... Args>
	struct Invoker < InvokerStorage, R(T::*)(Args...) const >
	{
		typedef R(*DoInvokeType)(InvokerStorageBase*, Args...);

		static R DoInvoke(InvokerStorageBase* base, Args... args)
		{
			InvokerStorage* call = static_cast<InvokerStorage*>(base);
			return ((call->m_instance)->*call->m_runnable)(std::forward<Args>(args)...);
		}
	};
	template <typename InvokerStorage, typename R, typename T, typename... Args>
	struct Invoker < InvokerStorage, R(T::*)(Args...) volatile >
	{
		typedef R(*DoInvokeType)(InvokerStorageBase*, Args...);

		static R DoInvoke(InvokerStorageBase* base, Args... args)
		{
			InvokerStorage* call = static_cast<InvokerStorage*>(base);
			return ((call->m_instance)->*call->m_runnable)(std::forward<Args>(args)...);
		}
	};
	template <typename InvokerStorage, typename R, typename T, typename... Args>
	struct Invoker < InvokerStorage, R(T::*)(Args...) const volatile >
	{
		typedef R(*DoInvokeType)(InvokerStorageBase*, Args...);

		static R DoInvoke(InvokerStorageBase* base, Args... args)
		{
			InvokerStorage* call = static_cast<InvokerStorage*>(base);
			return ((call->m_instance)->*call->m_runnable)(std::forward<Args>(args)...);
		}
	};
	//////////////////////////////////////////////////////////////////////
	template <typename FunctionType>
	class UnaryInvokerStorage : public InvokerStorageBase
	{
	public:
		typedef typename UnaryInvokerStorage SelfType;
		typedef typename FunctorTraits<FunctionType>::RunType RunType;
		typedef Invoker<SelfType, RunType> InvokerType;

		explicit UnaryInvokerStorage(const FunctionType& runnable)
			: m_runnable(runnable)
		{
		}

		virtual ~UnaryInvokerStorage()
		{
		}
		FunctionType m_runnable;
	};
	template <typename FunctionType, typename InstanceType>
	class BinaryInvokerStorage : public InvokerStorageBase
	{
	public:
		typedef typename BinaryInvokerStorage SelfType;
		typedef typename FunctorTraits<FunctionType>::RunType RunType;
		typedef Invoker<SelfType, RunType> InvokerType;
		explicit BinaryInvokerStorage(const FunctionType& runnable, const InstanceType& instance)
			: m_runnable(runnable),
			m_instance(instance)
		{
		}
		virtual ~BinaryInvokerStorage()
		{
		}
		FunctionType m_runnable;
		InstanceType m_instance;
	};
	//////////////////////////////////////////////////////////////////////////
	class CallbackBase
	{
	protected:
		typedef void(*InvokeFunctionBase)(void);
	public:
		explicit CallbackBase(InvokeFunctionBase invoke, InvokerStorageBase* storage);
		CallbackBase(CallbackBase&& other);
		CallbackBase(const CallbackBase& other);
		CallbackBase& operator=(CallbackBase&& other);
		CallbackBase& operator=(const CallbackBase& other);
		virtual ~CallbackBase();
	public:
		BOOL operator == (const CallbackBase& other) const;
		BOOL IsNull() const;
		void Reset();
	protected:
		TinyScopedReferencePtr<InvokerStorageBase> m_storage;
		InvokeFunctionBase m_invoke;
	};
	//////////////////////////////////////////////////////////////////////////
	template<typename InvokerStorage>
	class Caller
	{
	public:
		Caller(InvokerStorage* storage)
			: m_storage(static_cast<InvokerStorageBase*>(storage))
		{

		}
		mutable TinyScopedReferencePtr < InvokerStorageBase > m_storage;
	};

	template<typename T>
	Caller<T> MakeCaller(T* o)
	{
		return Caller<T>(o);
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename Runnable>
	class Callback;
	template<typename R, typename... Args>
	class Callback<R(Args...)> : public CallbackBase
	{
	private:
		typedef R(*InvokeFunction)(InvokerStorageBase*, Args...);
	public:
		Callback() : CallbackBase(NULL, NULL)
		{

		}
		virtual ~Callback()
		{

		}
		template<typename InvokerStorage>
		Callback(const Caller<InvokerStorage>& caller)
			: CallbackBase(reinterpret_cast<InvokeFunctionBase>(&InvokerStorage::InvokerType::DoInvoke), caller.m_storage.Ptr())
		{
			COMPILE_ASSERT((IsSameType<InvokeFunction, typename InvokerStorage::InvokerType::DoInvokeType>::Result), callback_type_not_match);
		}
		Callback(const Callback& callback)
			: CallbackBase(callback)
		{
		}
		Callback(Callback&& other)
			: CallbackBase(std::move(other))
		{
		}
		Callback& operator=(const Callback& other)
		{
			CallbackBase::operator=(other);
			return *this;
		}
		Callback& operator=(Callback&& other)
		{
			*static_cast<CallbackBase*>(this) = std::move(other);
			return *this;
		}
		BOOL operator == (const Callback& other) const
		{
			return CallbackBase::operator==(other);
		}
	public:
		R operator()(Args... args) const
		{
			InvokeFunction invoke = reinterpret_cast<InvokeFunction>(m_invoke);
			return invoke(m_storage.Ptr(), std::forward<Args>(args)...);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template <typename FunctionType>
	Caller <UnaryInvokerStorage<FunctionType>> BindCallback(const FunctionType& runnable)
	{
		return MakeCaller(new UnaryInvokerStorage<FunctionType>(runnable));
	};
	//////////////////////////////////////////////////////////////////////////
	template <typename FunctionType, typename InstanceType>
	Caller<BinaryInvokerStorage<FunctionType, InstanceType>> BindCallback(const FunctionType& runnable, const InstanceType& instance)
	{
		return MakeCaller(new BinaryInvokerStorage<FunctionType, InstanceType>(runnable, instance));
	};
	typedef Callback<void(void)> Closure;
};

