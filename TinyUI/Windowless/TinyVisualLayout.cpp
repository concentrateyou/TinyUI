#include "../stdafx.h"
#include "TinyVisualCaption.h"
#include "TinyVisualManage.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualLayout::TinyVisualLayout(TinyVisual* spvis)
			:m_spvis(spvis)
		{

		}
		BOOL TinyVisualLayout::Add(TinyVisual* spvis)
		{
			if (m_items.Lookup(spvis) >= 0)
				return FALSE;
			return m_items.Add(spvis);
		}
		BOOL TinyVisualLayout::Remove(TinyVisual* spvis)
		{
			return m_items.Remove(spvis);
		}
		void TinyVisualLayout::RemoveAll()
		{
			m_items.RemoveAll();
		}
		//////////////////////////////////////////////////////////////////////////
		TinyHBoxLayout::TinyHBoxLayout(TinyVisual* spvis)
			: TinyVisualLayout(spvis)
		{

		}
		void TinyHBoxLayout::Layout()
		{

		}
		//////////////////////////////////////////////////////////////////////////
		TinyVBoxLayout::TinyVBoxLayout(TinyVisual* spvis)
			: TinyVisualLayout(spvis)
		{

		}
		void TinyVBoxLayout::Layout()
		{

		}
	}
}
