#include "stdafx.h"
#include "MSearchController.h"
#include "WICTexture.h"

namespace MShow
{
	MSearchController::MSearchController(MSearchWindow& view)
		:m_view(view)
	{

	}

	MSearchController::~MSearchController()
	{

	}

	BOOL MSearchController::Initialize()
	{
		return TRUE;
	}
}
