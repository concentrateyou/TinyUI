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
			visual->Add("2017 �＾������1", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u78.png");
			visual->Add("2017 �＾������2", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u82.png");
			visual->Add("2017 �＾������3", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u86.png");
			visual->Add("2017 �＾������4", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u90.png");
			visual->Add("2017 �＾������5", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u98.png");
			visual->Add("2017 �＾������6", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Add("2017 �＾������6", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Add("2017 �＾������7", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Add("2017 �＾������8", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Add("2017 �＾������9", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Add("2017 �＾������10", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Add("2017 �＾������11", "D:\\����̨����-���½�˵\\images\\����̨-��˵�ͻ���\\u102.png");
			visual->Invalidate();
		}
		return TRUE;
	}
}
