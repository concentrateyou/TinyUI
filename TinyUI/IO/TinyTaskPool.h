#pragma once
#include "TinyTaskBase.h"

namespace TinyUI
{
	namespace IO
	{
		class TaskRunner
		{
		public:
			TaskRunner();
			virtual ~TaskRunner();
			BOOL PostTask(const Closure& task);
			BOOL PostTask(const Closure& task, const Closure& reply);
			virtual bool PostDelayedTask(const Closure& task, DWORD dwMS) = 0;
			virtual bool PostDelayedTask(const Closure& task, DWORD dwMS, const Closure& reply) = 0;
		};
		/// <summary>
		/// ÈÎÎñ³Ø
		/// </summary>
		class TinyTaskPool
		{
		public:
			TinyTaskPool(DWORD dwSize);
		public:

		private:
			DWORD m_dwSize;
		};
	};
}

