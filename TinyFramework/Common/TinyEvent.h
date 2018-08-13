#pragma once
#include "TinyDelegate.h"

namespace TinyFramework
{
	template<typename S>
	class Event;
	template<typename... Args>
	class Event < void(Args...) >
	{
		using DelegateType = Delegate < void(Args...) >;
		DISALLOW_COPY_AND_ASSIGN(Event)
	public:
		Event() = default;
	public:
		void operator+= (const DelegateType& dType)
		{
			if (m_array.Lookup(dType) < 0)
			{
				m_array.Add(dType);
			}
		}
		void operator+= (DelegateType&& dType)
		{
			if (m_array.Lookup(dType) < 0)
			{
				m_array.Add(std::move(dType));
			}
		}
		void operator-= (const DelegateType& dType)
		{
			INT index = m_array.Lookup(dType);
			if (index >= 0)
			{
				m_array.RemoveAt(index);
			}
		};
		void operator-= (DelegateType&& dType)
		{
			INT index = m_array.Lookup(dType);
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
				DelegateType& fType = m_array[i];
				if (!fType.IsEmpty())
				{
					fType(std::forward<Args>(args)...);
				}
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
		void Reset()
		{
			m_array.RemoveAll();
		}
	private:
		TinyArray<DelegateType> m_array;
	};
};

