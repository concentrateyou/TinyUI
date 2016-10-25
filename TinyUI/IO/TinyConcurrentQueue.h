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
		/// http://www.cs.rochester.edu/research/synchronization/pseudocode/queues.html
		/// </summary>
	/*	template<class T>
		class TinyConcurrentQueue
		{
			DISALLOW_COPY_AND_ASSIGN(TinyConcurrentQueue)
		protected:
			class TinyPointer;
			class TinyNode : public __ITERATOR
			{
			public:
				T			 m_value;
				TinyPointer* m_pNext;
			public:
				TinyNode()
					:m_pNext(NULL)
				{

				}
				TinyNode(const T& value)
					:m_value(value),
					m_pNext(NULL)
				{

				}
			};
			class TinyPointer
			{
			public:
				TinyNode*	m_pNode;
				DWORD		m_count;
			public:
				TinyPointer()
					:m_pNode(NULL),
					m_count(0)
				{

				}
			};
		public:
			explicit TinyConcurrentQueue();
			~TinyConcurrentQueue();
			void Push(const T& value);
			void Pop();
		protected:
			TinyNode*		m_pNode;
			TinyPointer*	m_pHEAD;
			TinyPointer*	m_pTAIL;
		};
		template<typename T>
		TinyConcurrentQueue<T>::TinyConcurrentQueue()
			:m_pNode(NULL),
			m_pHEAD(NULL),
			m_pTAIL(NULL)
		{
			m_pNode = new TinyNode();
			m_pHEAD->m_pNode = m_pNode;
			m_pTAIL->m_pNode = m_pNode;
		}
		template<typename T>
		TinyConcurrentQueue<T>::TinyConcurrentQueue()
		{

		}
		template<typename T>
		BOOL TinyConcurrentQueue<T>::Push(const T& value)
		{
			TinyNode* pNode = new TinyNode(value);
			for (;;)
			{
				TinyPointer* pLast = m_pTAIL;
				TinyNode* pNext = pLast->m_pNext;
				if (pLast == m_pTAIL)
				{
					if (pNext == NULL)
					{
						if CAS(&tail.ptr->next, next, <node, next.count + 1>)
							break;
					}
					else
					{
						CAS(&Q->Tail, tail, <next.ptr, tail.count + 1>)
					}
				}

			}
			CAS(&Q->Tail, tail, <node, tail.count + 1>);
		}
		template<typename T>
		BOOL TinyConcurrentQueue<T>::Pop()
		{
			for (;;)
			{
				TinyNode* pHEAD = m_pHEAD->m_pNode;
				TinyNode* pTAIL = m_pTAIL->m_pNode;
				TinyNode* pNEXT = pHEAD->m_pNode;
				if (pHEAD == pHEAD->m_pNode)
				{
					if head.ptr == tail.ptr
					{

					}
				}

			}
		}*/
	};
}

