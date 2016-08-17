#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualComboBox.h"

namespace TinyUI
{
	namespace Windowless
	{
		TinyVisualComboBox::TinyVisualComboBox(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			:TinyVisual(spvisParent, vtree)
		{

		}
		TinyVisualComboBox::~TinyVisualComboBox()
		{

		}
		TinyString TinyVisualComboBox::RetrieveTag() const
		{
			return TinyVisualTag::COMBOBOX;
		}

		BOOL TinyVisualComboBox::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			return TRUE;
		}
		HRESULT	TinyVisualComboBox::OnCreate()
		{
			TinySize size = this->GetSize();
			m_input = m_document->Create<TinyVisualButton>(size.cx - 19, (size.cy - 18) / 2, 19, 18, this);
			m_input->SetStyleImage(DOWN, "D:\\Develop\\GitHub\\TinyUI\\Debug\\Resource\\combobox\\inputbtn_down.png");
			m_input->SetStyleImage(NORMAL, "D:\\Develop\\GitHub\\TinyUI\\Debug\\Resource\\combobox\\inputbtn_normal.png");
			m_input->SetStyleImage(HIGHLIGHT, "D:\\Develop\\GitHub\\TinyUI\\Debug\\Resource\\combobox\\inputbtn_highlight.png");
			m_onClick.Reset(new Delegate<void(EventArgs&)>(this, &TinyVisualComboBox::OnInputClick));
			m_input->EVENT_Click += m_onClick;

			SetStyleImage(DOWN, "D:\\Develop\\GitHub\\TinyUI\\Debug\\Resource\\combobox\\combobox_down.png");
			SetStyleImage(NORMAL, "D:\\Develop\\GitHub\\TinyUI\\Debug\\Resource\\combobox\\combobox_normal.png");
			SetStyleImage(HIGHLIGHT, "D:\\Develop\\GitHub\\TinyUI\\Debug\\Resource\\combobox\\combobox_hover.png");
			return TinyVisual::OnCreate();
		}
		HRESULT TinyVisualComboBox::OnDestory()
		{
			m_input->EVENT_Click -= m_onClick;
			return TinyVisual::OnDestory();
		}

		void TinyVisualComboBox::OnInputClick(EventArgs& args)
		{
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(this->Handle(), &pos);
			//TODO
		}
	}
}
