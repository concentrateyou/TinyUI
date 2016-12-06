#pragma once
#include "TinyTaskBase.h"

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// 任务项
		/// </summary>
		class TaskItem
		{

		};
		/// <summary>
		/// 任务池
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

