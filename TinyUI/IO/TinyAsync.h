#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"

namespace TinyUI
{
	namespace IO
	{
		template<typename T>
		class TinyFuture
		{
			typedef T ValueType;
			DISALLOW_COPY_AND_ASSIGN(TinyFuture)
		public:
			TinyFuture(TinyFuture&&);
			TinyFuture& operator=(TinyFuture&&);
			~TinyFuture();
		};
		template<typename T>
		TinyFuture<T>::TinyFuture(TinyFuture&&)
		{

		}
		template<typename T>
		TinyFuture<T>& TinyFuture<T>::operator=(TinyFuture&&)
		{
			return *this;
		}
		template<typename T>
		TinyFuture<T>::~TinyFuture()
		{

		}
	};
}

