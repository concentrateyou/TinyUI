#pragma once
#include "MShowCommon.h"
#include "MPlayView.h"
#include "Control/TinyMenu.h"
#include "MElement.h"
using namespace TinyUI;

namespace MShow
{
	class MPlayView;
	/// <summary>
	/// ²¥·Å
	/// </summary>
	class MPlayController
	{
		DISALLOW_COPY_AND_ASSIGN(MPlayController)
	public:
		MPlayController(MPlayView& view);
		virtual ~MPlayController();
	private:
		MPlayView&	m_view;
	};
}


