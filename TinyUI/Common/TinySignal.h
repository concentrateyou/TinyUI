#pragma once
#include "../Common/TinyCollection.h"

namespace TinyUI
{
	//类似QT信号槽机制
	template<class Policy>
	class TinySignalBase : public Policy
	{

	};
}

