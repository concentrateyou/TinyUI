#pragma once
#include "TinyVisual.h"
#include "../IO/TinyIO.h"
#include "../XML/TinyXMLParse.h"

namespace TinyUI
{
	namespace Windowless
	{
		class TinyVisual;
		class TinyVisualTree;
		/// <summary>
		/// 可视化元素管理
		/// </summary>
		class TinyVisualParse : public TinyObject
		{
			DECLARE_DYNAMIC(TinyVisualParse)
		public:
			TinyVisualParse();
			~TinyVisualParse();
			BOOL LoadFile(LPCSTR pzFile);
			BOOL BuildVisualTree(TinyVisualTree* ps);
		private:
			TiXmlDocument	m_doc;
		};
	};
}



