#pragma once
#include "../Common/TinyCollection.h"
#include <thread>
#include <future> 

namespace TinyUI
{
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

