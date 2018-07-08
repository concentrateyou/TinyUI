#include "../stdafx.h"
#include "../Common/TinyString.h"
#include "TinyVisualCommon.h"
#include "TinyVisualManage.h"
#include "TinyVisualWindowless.h"
#include "TinyVisualComboBoxItem.h"

namespace TinyFramework
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualComboBoxItem, TinyVisual);

		TinyVisualComboBoxItem::TinyVisualComboBoxItem()
		{

		}
		TinyVisualComboBoxItem::TinyVisualComboBoxItem(TinyVisual* spvisParent, TinyVisualDocument* document)
			:TinyVisual(spvisParent, document)
		{

		}
		TinyVisualComboBoxItem::~TinyVisualComboBoxItem()
		{

		}

		TinyString TinyVisualComboBoxItem::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOXITEM;
		}
		BOOL TinyVisualComboBoxItem::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TinyVisual::OnDraw(hDC, rcPaint);
		}
	}
}
