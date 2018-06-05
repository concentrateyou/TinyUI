#pragma once
#include "TinyVisual.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class IVisualLayout
		{
		public:
			virtual BOOL Layout() = 0;
			virtual BOOL Calculate(const TinySize& _new, const TinySize& _old) = 0;
		};
		/// <summary>
		/// ´¹Ö±²¼¾Ö
		/// </summary>
		class TinyVisualVLayout : public IVisualLayout
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualVLayout)
		public:
			BOOL Layout() OVERRIDE;
			BOOL Calculate(const TinySize& _new, const TinySize& _old) OVERRIDE;
		};

		class TinyVisualHLayout : public IVisualLayout
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualHLayout)
		public:
			BOOL Layout() OVERRIDE;
			BOOL Calculate(const TinySize& _new, const TinySize& _old) OVERRIDE;
		};
	}
}



