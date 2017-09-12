#pragma once
#include "../Common/TinyCollection.h"
#include "../Common/TinyDelegate.h"
#include <memory>
using namespace std;

namespace TinyUI
{
	template<typename S>
	class SIGNAL;
	template<typename... Args>
	class SIGNAL < void(Args...) >
	{
		using DelegateType = Delegate < void(Args...) >;
		DISALLOW_COPY_AND_ASSIGN(SIGNAL)
	public:
		SIGNAL() = default;
	public:
		template<class FunctionType>
		void Connect(const FunctionType& fType)
		{
			DelegateType dType(fType);
			if (m_array.Lookup(dType) < 0)
			{
				m_array.Add(std::move(dType));
			}
		}

		template<class InstanceType, class FunctionType>
		void Connect(const InstanceType& iType, const FunctionType& fType)
		{
			DelegateType dType(iType, fType);
			if (m_array.Lookup(dType) < 0)
			{
				m_array.Add(std::move(dType));
			}
		}
		template<class FunctionType>
		void Disconnect(const FunctionType& fType)
		{
			DelegateType dType(fType);
			INT index = m_array.Lookup(dType);
			if (index >= 0)
			{
				m_array.RemoveAt(index);
			}
		};

		template<class InstanceType, class FunctionType>
		void Disconnect(const InstanceType& iType, const FunctionType& fType)
		{
			DelegateType dType(iType, fType);
			INT index = m_array.Lookup(dType);
			if (index >= 0)
			{
				m_array.RemoveAt(index);
			}
		};

		void DisconnectAll()
		{
			m_array.RemoveAll();
		}

		void operator()(Args... args)
		{
			INT size = m_array.GetSize();
			for (INT i = 0; i < size; i++)
			{
				DelegateType& fType = m_array[i];
				fType(args...);
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
		TinyArray<DelegateType> m_array;
	};
}

