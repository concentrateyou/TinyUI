#pragma once
#include "TinyVisual.h"
#include "../IO/TinyIO.h"
#include "../XML/TinyXMLParse.h"
#include "../Common/TinyCollection.h"
#include "TinyVisualCommon.h"
#include "TinyVisualWindow.h"

namespace TinyFramework
{
	namespace Windowless
	{
		class TinyVisualDocument;
		/// <summary>
		/// 资源管理
		/// </summary>
		class TinyVisualResource
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualResource)
		public:
			TinyVisualResource();
			~TinyVisualResource();
			static TinyVisualResource& GetInstance();
			BOOL Load(LPCSTR pzFile);
			string GetDefaultPath();
		public:
			BOOL		Add(TinyImage* image);
			TinyImage*	Add(const TinyString& szName, const TinyString& szFile, const TinyRectangle& center);
			void		Remove(const TinyString& szName);
			void		Remove(TinyImage* image);
			void		RemoveAll();
			TinyImage* operator[](const TinyString& szName);
		public:
			BOOL		Register(const TinyString& tag, const TinyString& value);
			void		Unregister(const TinyString& tag);
			TinyString	GetClassName(const TinyString& tag);
		private:
			static void BuildContext(const TiXmlNode* pXMLNode);
			static void BuildImage(const TiXmlNode* pXMLNode);
			static TinyRectangle GetRectangle(const TinyString& str);
		private:
			string							m_szPath;
			TinyLock						m_lock;
			TiXmlDocument					m_doc;
			TinyArray<TinyImage*>			m_images1;//动态
			TinyMap<TinyString, TinyImage*> m_images2;//静态
			TinyMap<TinyString, TinyString>	m_types;
		};
	};
}



