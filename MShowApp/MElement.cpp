#include "stdafx.h"
#include "MElement.h"

namespace MShow
{
	IMPLEMENT_DYNAMIC(MElement, TinyObject);

	MElement::MElement()
	{
	}


	MElement::~MElement()
	{
	}

	void MElement::SetPosition(const TinyPoint& position)
	{
		m_trackerRect.SetPosition(position);
	}
	TinyPoint MElement::GetPosition() const
	{
		return m_trackerRect.Position();
	}
	void MElement::SetScale(const TinySize& scale)
	{
		m_trackerRect.SetSize(scale);
	}
	void MElement::SetSize(const TinySize& size)
	{
		m_size = size;
	}
	TinySize MElement::GetScale() const
	{
		return m_trackerRect.Size();
	}
	TinySize MElement::GetSize() const
	{
		return m_size;
	}
	BOOL MElement::PtInRect(const TinyPoint& pos)
	{
		return m_trackerRect.PtInRect(pos);
	}
	void MElement::SetName(const TinyString& name)
	{
		m_name = std::move(name);
	}
	TinyString MElement::GetName() const
	{
		return m_name;
	}
}