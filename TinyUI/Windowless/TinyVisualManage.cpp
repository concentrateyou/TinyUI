#include "../stdafx.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindow.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualParse::TinyVisualParse()
		{

		}
		TinyVisualParse::~TinyVisualParse()
		{

		}
		BOOL TinyVisualParse::LoadFile(LPCSTR pzFile)
		{
			ASSERT(pzFile);
			return m_doc.LoadFile(pzFile);
		}
		BOOL TinyVisualParse::BuildVisualTree(TinyVisualTree* ps)
		{
			TiXmlElement *ele = m_doc.RootElement();
			if (!ele && !strcasecmp(ele->Value(), TinyVisualWindow::Tag))
				return FALSE;
			//获得根节点
			TinyVisual* pWindow = ps->GetParent(NULL);
			if (!pWindow) return FALSE;
			pWindow->ParseVisual(ele);
			return FALSE;
		}
	};
}
