#pragma once
#include "TinyTaskBase.h"

namespace TinyUI
{
	namespace IO
	{
		/// <summary>
		/// ������
		/// </summary>
		class TaskItem
		{

		};
		/// <summary>
		/// �����
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

