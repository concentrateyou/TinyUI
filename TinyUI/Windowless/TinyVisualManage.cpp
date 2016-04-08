#include "../stdafx.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindow.h"

namespace TinyUI
{
	namespace Windowless
	{
		const CHAR* TinyVisualPoperty::NAME = TEXT("name");
		const CHAR* TinyVisualPoperty::TEXT = TEXT("text");
		const CHAR* TinyVisualPoperty::TOOLTIP = TEXT("tooltip");
		const CHAR* TinyVisualPoperty::MAXSIZE = TEXT("maxsize");
		const CHAR* TinyVisualPoperty::MINSIZE = TEXT("minsize");
		const CHAR* TinyVisualPoperty::SIZE = TEXT("size");
		const CHAR* TinyVisualPoperty::VISIBLE = TEXT("visible");
		const CHAR* TinyVisualPoperty::ENABLE = TEXT("enable");
		//////////////////////////////////////////////////////////////////////////
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
			pWindow->ParsePropertys(ele);
			return FALSE;
		}
	};
}
