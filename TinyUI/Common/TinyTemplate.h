#pragma once
#include "TinyCommon.h"

namespace TinyUI
{
	namespace Template
	{
		/*template <typename Functor>
		class RunnableAdapter;

		template <typename R>
		class RunnableAdapter < R(*)() >
		{
		public:
		typedef R(RunType)();

		explicit RunnableAdapter(R(*function)())
		: function_(function)
		{
		}

		R Run()
		{
		return function_();
		}

		private:
		R(*function_)();
		};

		template <typename R, typename T>
		class RunnableAdapter < R(T::*)() >
		{
		public:
		typedef R(RunType)(T*);
		typedef true_type IsMethod;

		explicit RunnableAdapter(R(T::*method)())
		: method_(method)
		{
		}

		R Run(T* object)
		{
		return (object->*method_)();
		}

		private:
		R(T::*method_)();
		};

		template <typename R, typename T>
		class RunnableAdapter < R(T::*)() const >
		{
		public:
		typedef R(RunType)(const T*);
		typedef true_type IsMethod;

		explicit RunnableAdapter(R(T::*method)() const)
		: method_(method)
		{
		}

		R Run(const T* object)
		{
		return (object->*method_)();
		}

		private:
		R(T::*method_)() const;
		};

		template <typename R, typename A1>
		class RunnableAdapter < R(*)(A1) >
		{
		public:
		typedef R(RunType)(A1);

		explicit RunnableAdapter(R(*function)(A1))
		: function_(function) {
		}

		R Run(A1 a1)
		{
		return function_(a1);
		}

		private:
		R(*function_)(A1);
		};

		template <typename R, typename T, typename A1>
		class RunnableAdapter < R(T::*)(A1) >
		{
		public:
		typedef R(RunType)(T*, A1);

		explicit RunnableAdapter(R(T::*method)(A1))
		: method_(method)
		{
		}

		R Run(T* object, A1 a1)
		{
		return (object->*method_)(a1);
		}

		private:
		R(T::*method_)(A1);
		};

		template <typename R, typename T, typename A1>
		class RunnableAdapter < R(T::*)(A1) const >
		{
		public:
		typedef R(RunType)(const T*, A1);
		typedef true_type IsMethod;

		explicit RunnableAdapter(R(T::*method)(A1) const)
		: method_(method) {
		}

		R Run(const T* object, A1 a1)
		{
		return (object->*method_)(a1);
		}

		private:
		R(T::*method_)(A1) const;
		};

		template <typename Sig>
		struct FunctionTraits;

		template <typename R>
		struct FunctionTraits < R() >
		{
		typedef R ReturnType;
		};

		template <typename R, typename A1>
		struct FunctionTraits < R(A1) >
		{
		typedef R ReturnType;
		typedef A1 A1Type;
		};

		template <typename R, typename A1, typename A2>
		struct FunctionTraits < R(A1, A2) >
		{
		typedef R ReturnType;
		typedef A1 A1Type;
		typedef A2 A2Type;
		};

		template <typename T>
		struct FunctorTraits
		{
		typedef RunnableAdapter<T> RunnableType;
		typedef typename RunnableType::RunType RunType;
		};

		template <typename T>
		typename FunctorTraits<T>::RunnableType MakeRunnable(const T& t) {
		return RunnableAdapter<T>(t);
		}

		template <typename T>
		struct FunctorTraits < Callback<T> > {
		typedef Callback<T> RunnableType;
		typedef typename Callback<T>::RunType RunType;
		};

		template <typename Sig>
		class Callback;

		class BindStateBase : public TinyReference < BindStateBase >
		{
		protected:
		friend class TinyReference < BindStateBase > ;
		virtual ~TinyReference() {}
		};

		template <typename Runnable, typename RunType, typename BoundArgsType>
		struct BindState;

		class  CallbackBase
		{
		public:
		bool is_null() const;
		void Reset();
		protected:
		typedef void(*InvokeFuncStorage)(void);
		bool Equals(const CallbackBase& other) const;
		explicit CallbackBase(BindStateBase* bind_state);
		~CallbackBase();
		TinyScopedReferencePtr<BindStateBase> bind_state_;
		InvokeFuncStorage polymorphic_invoke_;
		};*/

	}
}
