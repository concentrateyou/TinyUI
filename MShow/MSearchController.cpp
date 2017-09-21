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
			visual->Add("2017 秋季发布会1", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u78.png");
			visual->Add("2017 秋季发布会2", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u82.png");
			visual->Add("2017 秋季发布会3", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u86.png");
			visual->Add("2017 秋季发布会4", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u90.png");
			visual->Add("2017 秋季发布会5", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u98.png");
			visual->Add("2017 秋季发布会6", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Add("2017 秋季发布会6", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Add("2017 秋季发布会7", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Add("2017 秋季发布会8", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Add("2017 秋季发布会9", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Add("2017 秋季发布会10", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Add("2017 秋季发布会11", "D:\\导播台三期-赛事解说\\images\\导播台-解说客户端\\u102.png");
			visual->Invalidate();
		}
		return TRUE;
	}
}
