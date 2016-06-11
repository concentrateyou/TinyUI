#pragma once
#include "TinyVisualCommon.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisual;
		/// <summary>
		/// 布局类 
		/// </summary>
		class TinyVisualLayout
		{
		public:
			TinyVisualLayout(TinyVisual* spvis);
		public:
			virtual BOOL Add(TinyVisual* spvis);
			virtual BOOL Remove(TinyVisual* spvis);
			virtual void RemoveAll();
			virtual void Layout() = 0;
		private:
			TinyVisual* m_spvis;
			TinyArray<TinyVisual*> m_items;
		};
		/// <summary>
		/// 水平布局
		/// </summary>
		class TinyHBoxLayout : public TinyVisualLayout
		{
		public:
			TinyHBoxLayout(TinyVisual* spvis);
		public:
			void Layout() OVERRIDE;
		};
		/// <summary>
		/// 垂直布局
		/// </summary>
		class TinyVBoxLayout : public TinyVisualLayout
		{
		public:
			TinyVBoxLayout(TinyVisual* spvis);
		public:
			void Layout() OVERRIDE;
		};
	}
}



