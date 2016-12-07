#pragma once
#include "../Common/TinyCollection.h"
#include <thread>
#include <future> 

namespace TinyUI
{
	template<class _Ty>
	class AssociateState;

	template<class _Ty>
	class AssociateState
	{
		typedef _Ty SelfType;
	private:
		_Ty		m_result;
	};

	template<class _Ty >
	class TinyFuture
	{
		DISALLOW_COPY_AND_ASSIGN(TinyFuture)
	public:
		TinyFuture();
		TinyFuture(TinyFuture&& _Other);
		TinyFuture& operator=(TinyFuture&& _Right);
		~TinyFuture();
	};

	template<class _Ty >
	class TinyPromise
	{
		DISALLOW_COPY_AND_ASSIGN(TinyPromise)
	};
}

