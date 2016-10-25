#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyCore.h"
#include "../Common/TinyCallback.h"
#include "../Common/TinyEvent.h"
#include <string>
#include <algorithm>
using namespace std;

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 多线程安全队列
		/// http://www.ibm.com/developerworks/cn/aix/library/au-multithreaded_structures2/index.html
		/// </summary>
		template<class T>
		class TinyConcurrentQueue
		{
			DISALLOW_COPY_AND_ASSIGN(TinyConcurrentQueue)
		protected:
			class TinyNode : public __ITERATOR
			{
			public:
				T			m_value;
				TinyNode*	m_pNext;
			public:
				TinyNode(const T& value)
					:m_value(value),
					m_pNext(NULL)
				{

				}
			};
		public:
			explicit TinyConcurrentQueue();
			~TinyConcurrentQueue();
			BOOL Push(const T& value);

		protected:
			TinyNode*	m_pNode;
			TinyNode*	m_pFirst;
			TinyNode*	m_pLast;
		};
		template<typename T>
		TinyConcurrentQueue<T>::TinyConcurrentQueue()
			:m_pNode(NULL),
			m_pFirst(NULL),
			m_pLast(NULL)
		{

		}
		template<typename T>
		TinyConcurrentQueue<T>::TinyConcurrentQueue()
		{

		}
		template<typename T>
		BOOL TinyConcurrentQueue<T>::Push(const T& value)
		{
			return TRUE;
		}
	};
}

