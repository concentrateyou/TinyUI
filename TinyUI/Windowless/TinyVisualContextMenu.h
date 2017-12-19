#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyUI
{
	namespace Windowless
	{
		/// <summary>
		/// �˵���
		/// </summary>
		class TinyVisualMenuItem : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualMenuItem)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualMenuItem)
		protected:
			TinyVisualMenuItem();
			TinyVisualMenuItem(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			TinyString RetrieveTag() const OVERRIDE;
		};
		/// <summary>
		/// ���ӻ������Ĳ˵�
		/// </summary>
		class TinyVisualContextMenu : public TinyVisual
		{
			friend class TinyVisualDocument;
			DECLARE_DYNAMIC(TinyVisualContextMenu)
			DISALLOW_COPY_AND_ASSIGN(TinyVisualContextMenu)
		protected:
			TinyVisualContextMenu();
			TinyVisualContextMenu(TinyVisual* spvisParent, TinyVisualDocument* vtree);
		public:
			TinyString RetrieveTag() const OVERRIDE;
		public:
			 
		private:
			TinyArray<TinyVisualMenuItem*>	m_items;
		};
	}
}



