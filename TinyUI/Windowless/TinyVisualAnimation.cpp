#include "../stdafx.h"
#include "TinyVisualDocument.h"
#include "TinyVisualManage.h"
#include "TinyVisualAnimation.h"

namespace TinyUI
{
	namespace Windowless
	{
		IMPLEMENT_DYNCREATE(TinyVisualAnimation, TinyVisual);

		TinyVisualAnimation::TinyVisualAnimation()
			:m_hTimer(NULL),
			m_index(0),
			m_animation(NULL)
		{
		}
		TinyVisualAnimation::TinyVisualAnimation(TinyVisual* spvisParent, TinyVisualDocument* vtree)
			: TinyVisual(spvisParent, vtree)
		{
		}
		TinyVisualAnimation::~TinyVisualAnimation()
		{

		}
		TinyString TinyVisualAnimation::RetrieveTag() const
		{
			return TinyVisualTag::ANIMATION;
		}

		BOOL TinyVisualAnimation::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualProperty::IMAGE.STR()) == 0)
			{
				this->SetAnimateImage(value.STR());
				return TRUE;
			}
			return	TinyVisual::SetProperty(name, value);
		}

		void TinyVisualAnimation::SetAnimateImage(const TinyString& szName)
		{
			m_animation = TinyVisualResource::GetInstance()[szName];
		}

		void TinyVisualAnimation::SetAnimateImage(TinyImage* image)
		{
			m_animation = image;
			if (m_animation != NULL)
			{
				TinyVisualResource::GetInstance().Add(m_animation);
			}
		}

		BOOL TinyVisualAnimation::BeginAnimate()
		{
			EndAnimate();
			if (m_animation != NULL && m_animation->GetCount() > 0)
			{
				INT delay = m_animation->GetDelay(m_index);
				if (delay <= 0)
				{
					delay = 40;
				}
				m_hTimer = TinyApplication::GetInstance()->GetTimers().Register(&TinyVisualAnimation::OnTimer, this, delay, 1);
				return m_hTimer != NULL;
			}
			return FALSE;
		}

		VOID CALLBACK TinyVisualAnimation::OnTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired)
		{
			TinyVisualAnimation* spvis = static_cast<TinyVisualAnimation*>(lpParam);
			ASSERT(spvis);
			ASSERT(spvis->m_hTimer != NULL);
			spvis->m_index = ((spvis->m_index + 1) == spvis->m_animation->GetCount()) ? 0 : (spvis->m_index + 1);
			INT delay = spvis->m_animation->GetDelay(spvis->m_index);
			if (delay <= 0)
			{
				delay = 40;
			}
			TinyApplication::GetInstance()->GetTimers().Change(spvis->m_hTimer, delay, 1);
			spvis->Invalidate();
		}

		BOOL TinyVisualAnimation::EndAnimate()
		{
			BOOL bRes = TRUE;
			if (m_hTimer != NULL)
			{
				bRes = TinyApplication::GetInstance()->GetTimers().Unregister(m_hTimer);
				m_hTimer = NULL;
				Sleep(500);
			}
			m_index = 0;
			return bRes;
		}

		BOOL TinyVisualAnimation::OnDraw(HDC hDC, const RECT& rcPaint)
		{
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			if (m_animation != NULL && !m_animation->IsEmpty())
			{
				TinyRectangle srcRect = m_animation->GetRectangle();
				canvas.DrawBitmap(m_animation->GetHBITMAP(m_index), clip, srcRect);
			}
			return TRUE;
		}
	}
}
