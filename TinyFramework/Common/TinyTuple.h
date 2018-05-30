#pragma once
#include "../Common/TinyCollection.h"
#include <algorithm>
#include <typeinfo>
using namespace std;

namespace TinyFramework
{
	/// <summary>
	/// Any数据结构参考Boost
	/// </summary>
	class TinyAny
	{
	public:
		template<class ValueType>
		TinyAny(const ValueType& my)
			:m_myP(new Any<ValueType>(my))
		{
			TRACE("TinyAny(const ValueType& my)\n");
		}
		TinyAny(const TinyAny& other)
			:m_myP(other.m_myP ? other.m_myP->DoClone() : NULL)
		{
			TRACE("TinyAny(const TinyAny& other)\n");
		}
		TinyAny& operator=(const TinyAny& other)
		{
			TRACE("TinyAny& operator=(const TinyAny& other)\n");
			m_myP = other.m_myP;
			return *this;
		}
		template<class ValueType>
		TinyAny& operator=(const ValueType& my)
		{
			TRACE("TinyAny& operator=(const ValueType& my)\n");
			m_myP = TinyAny(my).m_myP;
			return *this;
		}
		~TinyAny()
		{
			TRACE("~TinyAny\n");
			SAFE_DELETE(m_myP);
		}
		BOOL IsEmpty() const
		{
			return m_myP == NULL;
		}
		const type_info& GetType() const
		{
			return m_myP ? m_myP->GetType() : typeid(void);
		}
	private:
		class AnyBase
		{
		public:
			virtual ~AnyBase(){};
			virtual AnyBase* DoClone() const = 0;
			virtual const type_info& GetType() const = 0;
		};
		template<class ValueType>
		class Any : public AnyBase
		{
			DISALLOW_COPY_AND_ASSIGN(Any)
		public:
			Any(const ValueType& my)
				:m_my(my)
			{
				TRACE("Any(const ValueType& my)\n");
			}
			virtual ~Any()
			{
				TRACE("~Any\n");
			}
			AnyBase* DoClone() const override
			{
				return new Any(m_my);
			}
			const type_info& GetType() const
			{
				return typeid(ValueType);
			}
		public:
			ValueType m_my;
		};
	private:
		AnyBase* m_myP;
	};
}

