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
		TinyVisualList* visual = static_cast<TinyVisualList*>(m_view.GetDocument()->GetVisualByName("programs"));
		if (visual != NULL)
		{
			visual->Invalidate();
		}
		return TRUE;
	}
}
