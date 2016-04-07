#include "../stdafx.h"
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
		const CHAR* TinyVisualTag::WINDOW = TEXT("window");
		const CHAR* TinyVisualTag::CAPTION = TEXT("caption");
		const CHAR* TinyVisualTag::SYSBUTTON = TEXT("sysbutton");
		const CHAR* TinyVisualTag::BUTTON = TEXT("button");
		const CHAR* TinyVisualTag::COMBOBOX = TEXT("combobox");
		const CHAR* TinyVisualTag::TAB = TEXT("tab");
		const CHAR* TinyVisualTag::LIST = TEXT("list");
		const CHAR* TinyVisualTag::TREEVIEW = TEXT("treeview");
		const CHAR* TinyVisualTag::TOOLBAR = TEXT("toolbar");
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
			if (!ele && !strcasecmp(ele->Value(), TinyVisualTag::WINDOW))
				return FALSE;

			return FALSE;
		}
	};
}
