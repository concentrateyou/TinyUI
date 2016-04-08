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
		class TinyVisualPoperty
		{
		public:
			//元素属性
			static const CHAR* NAME;
			static const CHAR* TEXT;
			static const CHAR* TOOLTIP;
			static const CHAR* MAXSIZE;
			static const CHAR* MINSIZE;
			static const CHAR* SIZE;
			static const CHAR* VISIBLE;
			static const CHAR* ENABLE;
		};
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



