#include "stdafx.h"
#include "MPlayController.h"

namespace MShow
{
	MPlayController::MPlayController(MPlayView& view)
		:m_view(view)
	{

	}

	MPlayController::~MPlayController()
	{

	}

	BOOL MPlayController::Initialize()
	{
		return TRUE;
	}
}
