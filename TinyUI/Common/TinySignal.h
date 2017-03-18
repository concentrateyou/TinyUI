#pragma once
#include "../Common/TinyCollection.h"
#include <set>
#include <list>

namespace TinyUI
{
	class ISignal;
	class ISlots;

	class ISignal
	{
	public:
		virtual ~ISignal() {};
		virtual void disconnect(ISlots* slots) = 0;
		virtual void duplicate(const ISlots* oldslots, ISlots* newslots) = 0;
	};
	class ISlots
	{
	public:
		ISlots() {};
		virtual ~ISlots() {};
		virtual void connect(ISignal* signal) = 0;
		virtual void disconnect(ISignal* signal) = 0;
	};

	template<class Policy>
	class TinySignalBase : public ISignal, public Policy
	{
	};

	template<class Policy, typename... Args>
	class ConnectionBase
	{
	public:
		virtual ~ConnectionBase() {};
		virtual ISlots* slots() const = 0;
		virtual void emit(Args...) = 0;
		virtual ConnectionBase* clone() = 0;
		virtual ConnectionBase* duplicate(ISlots* pnewdest) = 0;
	};

	template<class Policy>
	class TinySlots : public ISlots, public Policy
	{
	private:
		typedef std::set<ISignal*> signals;
		typedef signals::const_iterator const_iterator;
	public:
		TinySlots()
		{

		}
		TinySlots(const TinySlots& slots)
		{
			const_iterator begin = slots.m_signals.begin();
			const_iterator end = slots.m_signals.end();
			while (begin != end)
			{
				(*begin)->duplicate(&slots, this);
				m_signals.insert(*begin);
				++begin;
			}
		}
		void connect(ISignal* signal)
		{
			m_signals.insert(signal);
		}
		void disconnect(ISignal* signal)
		{
			if (signal == NULL)
			{
				const_iterator begin = m_signals.begin();
				const_iterator end = m_signals.end();
				while (begin != end)
				{
					(*begin)->disconnect(this);
					++begin;
				}
				m_signals.erase(m_signals.begin(), m_signals.end());
			}
			else
			{
				m_signals.erase(signal);
			}
		}
		virtual ~TinySlots()
		{
			disconnect(NULL);
		}
	private:
		signals m_signals;
	};
	template<class Policy, typename... Args>
	class TinySignalImpl : public TinySignalBase<Policy>
	{
	public:
		typedef std::list<ConnectionBase<Policy, Args...>*>  connections;
		TinySignalImpl()
		{
		}
		TinySignalImpl(const TinySignalImpl& s)
			: TinySignalBase<Policy>(s)
		{
			typename connections::const_iterator begin = s.m_connections.begin();
			typename connections::const_iterator end = s.m_connections.end();
			while (begin != end)
			{
				(*begin)->slots()->connect(this);
				m_connections.push_back((*begin)->clone());
				++begin;
			}
		}
		~TinySignalImpl()
		{
			disconnect(NULL);
		}
		BOOL empty()
		{
			typename connections::const_iterator begin = m_connections.begin();
			typename connections::const_iterator end = m_connections.end();
			return begin == end;
		}
		void disconnect(ISlots* slots)
		{
			typename connections::iterator begin = m_connections.begin();
			typename connections::iterator end = m_connections.end();
			if (slots == NULL)
			{
				while (begin != end)
				{
					(*begin)->slots()->disconnect(this);
					delete *begin;
					++begin;
				}
				m_connections.erase(m_connections.begin(), m_connections.end());
			}
			else
			{
				while (begin != end)
				{
					if ((*begin)->slots() == slots)
					{
						delete *begin;
						m_connections.erase(begin);
						slots->disconnect(this);
						return;
					}
					++begin;
				}
			}
		}
		void duplicate(const ISlots* oldslots, ISlots* newslots)
		{
			typename connections::iterator begin = m_connections.begin();
			typename connections::iterator end = m_connections.end();
			while (begin != end)
			{
				if ((*begin)->slots() == oldslots)
				{
					m_connections.push_back((*begin)->duplicate(newslots));
				}
				++begin;
			}
		}
	protected:
		connections m_connections;
	};
	template<class Instance, class Policy, typename... Args>
	class Connection : public ConnectionBase<Policy, Args...>
	{
	public:
		Connection()
			:m_instance(NULL),
			m_runnable(NULL)
		{

		}
		Connection(Instance* instance, void (Instance::*runnable)(Args...))
			:m_instance(instance),
			m_runnable(runnable)
		{

		}
		virtual ~Connection()
		{
		}
		virtual ConnectionBase<Policy, Args...>* clone()
		{
			return new Connection<Instance, Policy, Args...>(*this);
		}
		virtual ConnectionBase<Policy, Args...>* duplicate(ISlots* pnewdest)
		{
			return new Connection<Instance, Policy, Args...>((Instance *)pnewdest, m_runnable);
		}
		virtual void emit(Args... args)
		{
			(m_instance->*m_runnable)(args...);
		}
		virtual ISlots* slots() const
		{
			return m_instance;
		}
	private:
		Instance* m_instance;
		void (Instance::* m_runnable)(Args...);
	};

	template<class Policy, typename... Args>
	class TinySignal : public TinySignalImpl<Policy, Args...>
	{
	public:
		typedef TinySignalImpl<Policy, Args...> base;
		typedef typename base::connections connections_list;
		using base::m_connections;

		TinySignal()
		{

		}
		TinySignal(const TinySignal<Policy, Args...>& s)
			: TinySignalImpl<Policy, Args...>(s)
		{

		}

		template<class Instance>
		void connect(Instance* instance, void (Instance::*runable)(Args...))
		{
			Connection<Instance, Policy, Args...>* s = new Connection<Instance, Policy, Args...>(instance, runable);
			m_connections.push_back(s);
			instance->connect(this);
		}
		void emit(Args... args)
		{
			typename connections_list::const_iterator next, begin = m_connections.begin();
			typename connections_list::const_iterator end = m_connections.end();
			while (begin != end)
			{
				next = begin;
				++next;
				(*begin)->emit(args...);
				begin = next;
			}
		}
		void operator()(Args... args)
		{
			typename connections_list::const_iterator next, begin = m_connections.begin();
			typename connections_list::const_iterator end = m_connections.end();
			while (begin != end)
			{
				next = begin;
				++next;
				(*begin)->emit(args...);
				begin = next;
			}
		}
	};
}

