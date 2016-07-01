#pragma once
#include "TinyCommon.h"

namespace TinyUI
{
	namespace Template
	{

		/*enum class CopyMode
		{
			MoveOnly,
			Copyable,
		};

		template <typename Functor>
		class RunnableAdapter;

		template <typename R, typename... Args>
		class RunnableAdapter < R(*)(Args...) >
		{
		public:
			typedef R ReturnType;
			typedef R(*RunType)(Args...);
			explicit RunnableAdapter(RunType s)
				: m_s(s)
			{
			}
			template <typename... RunArgs>
			R DoInvoke(RunArgs&&... args)
			{
				return m_s(std::forward<RunArgs>(args)...);
			}
		private:
			RunType m_s;
		};

		template <typename R, typename T, typename... Args>
		class RunnableAdapter < R(T::*)(Args...) >
		{
		public:
			typedef R ReturnType;
			typedef R(T::*RunType)(Args...);
			explicit RunnableAdapter(RunType s)
				: m_s(s)
			{
			}
			template <typename Instance, typename... RunArgs>
			R DoInvoke(Instance&& instance, RunArgs&&... args)
			{
				T& receiver = *instance;
				return (receiver.*m_s)(std::forward<RunArgs>(args)...);
			}
		private:
			RunType m_s;
		};

		template <typename R, typename T, typename... Args>
		class RunnableAdapter < R(T::*)(Args...) const >
		{
		public:
			typedef R ReturnType;
			typedef R(T::*RunType)(Args...) const;
			explicit RunnableAdapter(RunType s)
				: m_s(s)
			{
			}
			template <typename Instance, typename... RunArgs>
			R DoInvoke(Instance&& instance, RunArgs&&... args)
			{
				const T& myT = *instance;
				return (myT.*m_s)(std::forward<RunArgs>(args)...);
			}
		private:
			RunType m_s;
		};

		template <typename T>
		struct FunctorTraits
		{
			using RunnableType = RunnableAdapter < T > ;
			using RunType = typename RunnableType::RunType;
		};

		template <typename T>
		typename FunctorTraits<T>::RunnableType MakeRunnable(const T& myT)
		{
			return RunnableAdapter<T>(myT);
		}


		class InvokerStorageBase : public TinyReference < InvokerStorageBase >
		{
		public:
			virtual ~InvokerStorageBase();
		};

		template <typename Signature, CopyMode copy_mode = CopyMode::Copyable>
		class Callback;

		template <CopyMode copy_mode>
		class CallbackBase;

		template <>
		class  CallbackBase < CopyMode::MoveOnly >
		{
		public:
			CallbackBase(CallbackBase&& c);
			CallbackBase& operator=(CallbackBase&& c);
			BOOL IsEmpty() const { return m_storage.Ptr() == NULL; }
			void Reset();

		protected:
			using InvokeFuncStorage = void(*)();
			BOOL Equals(const CallbackBase& other) const;
			explicit CallbackBase(InvokerStorageBase* storage);
			~CallbackBase();

			TinyScopedReferencePtr<InvokerStorageBase> m_storage;
			InvokeFuncStorage polymorphic_invoke_ = nullptr;
		};

		template <>
		class  CallbackBase<CopyMode::Copyable> : public CallbackBase < CopyMode::MoveOnly >
		{
		public:
			CallbackBase(const CallbackBase& c);
			CallbackBase(CallbackBase&& c);
			CallbackBase& operator=(const CallbackBase& c);
			CallbackBase& operator=(CallbackBase&& c);
		protected:
			explicit CallbackBase(InvokerStorageBase* storage) : CallbackBase<CopyMode::MoveOnly>(storage)
			{

			}
			~CallbackBase() {}
		};*/

	}
}
