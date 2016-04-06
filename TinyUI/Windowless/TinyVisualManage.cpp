#include "../stdafx.h"
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
		const CHAR* TinyVisualTag::WINDOW = TEXT("window");
		const CHAR* TinyVisualTag::CAPTION = TEXT("caption");
		const CHAR* TinyVisualTag::SYSBUTTON = TEXT("sysbutton");
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
