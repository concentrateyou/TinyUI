#pragma once
#include "TinyDelegate.h"

namespace TinyUI
{
	template<typename S>
	class Event;
	template<typename... Args>
	class Event < void(Args...) >
	{
		DISALLOW_COPY_AND_ASSIGN(Event)
		using DelegateType = Delegate < void(Args...) > ;
	public:
		void operator+= (DelegateType* ps)
		{
			if (m_array.Lookup(ps) < 0)
			{
				m_array.Add(ps);
			}
		}
		void operator-= (DelegateType* ps)
		{
			ASSERT(ps);
			INT index = m_array.Lookup(ps);
			if (index >= 0)
			{
				m_array.RemoveAt(index);
			}
		};
		void operator()(Args... args)
		{
			INT size = m_array.GetSize();
			for (INT i = 0; i < size; i++)
			{
				DelegateType* fType = static_cast<DelegateType*>(m_array[i]);
				if (fType)
				{
					(*fType)(args...);
				};
			};
		};
		BOOL IsEmpty() const
		{
			return m_array.IsEmpty();
		}
		INT GetSize() const
		{
			return m_array.GetSize();
		};
	public:
		Event() = DEFAULT;
	private:
		TinyArray<DelegateType*> m_array;
	};
};

