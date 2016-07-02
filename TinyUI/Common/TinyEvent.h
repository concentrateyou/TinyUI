#pragma once
#include "TinyDelegate.h"

namespace TinyUI
{
	/// <summary>
	/// ÊÂ¼þÀà
	/// </summary>
	template<typename R>
	class Event;
	template<typename R, typename... Args>
	class Event < R(Args...) >
	{
	public:
		typedef typename Delegate<R(Args...)> DelegateType;
		void operator+= (DelegateType* ps)
		{
			COMPILE_ASSERT(IsVoidType<R>::Result != 0, return_type_must_void);
			if (m_array.Lookup(ps) < 0)
			{
				m_array.Add(ps);
			}
		}
		void operator-= (DelegateType* ps)
		{
			INT index = m_array.Lookup(ps);
			if (index >= 0)
			{
				m_array.RemoveAt(index);
			}
		};
		R operator()(Args... args)
		{
			INT size = m_array.GetSize();
			if (size == 1)
			{
				DelegateType* fType = static_cast<DelegateType*>(m_array[0]);
				if (fType)
				{
					return (*fType)(args...);
				};
			}
			for (INT i = 0; i < size; i++)
			{
				DelegateType* fType = static_cast<DelegateType*>(m_array[i]);
				if (fType)
				{
					(*fType)(args...);
				};
			};
		};
		BOOL Add(DelegateType* ps)
		{
			COMPILE_ASSERT(IsVoidType<R>::Result != 0, return_type_must_void);
			if (m_array.Lookup(ps) < 0)
			{
				return	m_array.Add(ps);
			}
		}
		INT GetSize() const
		{
			return m_array.GetSize();
		};
	private:
		TinyArray<DelegateType*> m_array;
	};
};

