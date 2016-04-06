#include "../stdafx.h"
#include "TinyVisualManage.h"

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
			return FALSE;
		}
	};
}
