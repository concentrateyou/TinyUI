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
			m_index(0)
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
			return TinyVisualTagConst::ANIMATION;
		}

		BOOL TinyVisualAnimation::SetProperty(const TinyString& name, const TinyString& value)
		{
			if (strcasecmp(name.STR(), TinyVisualPropertyConst::IMAGELIST.STR()) == 0)
			{
				TinyArray<TinyString> szFiles;
				vector<string> values;
				SplitString(value.STR(), ',', &values);
				for (INT i = 0;i < values.size();i++)
				{
					szFiles.Add(values[i].c_str());
				}
				this->SetAnimateImage(szFiles);
				return TRUE;
			}
			return	TinyVisual::SetProperty(name, value);
		}

		void TinyVisualAnimation::SetAnimateImage(TinyArray<TinyString>& szFiles)
		{
			for (INT i = 0;i < szFiles.GetSize();i++)
			{
				m_images.Add(TinyVisualResource::GetInstance()[szFiles[i]]);
			}
		}

		void TinyVisualAnimation::SetAnimateImage(TinyArray<TinyImage*>& images)
		{
			m_images = images;
		}

		BOOL TinyVisualAnimation::BeginAnimate()
		{
			EndAnimate();
			if (m_images.GetSize() > 0)
			{
				if (m_hTimer != NULL)
				{
					TinyApplication::GetInstance()->GetTimers().Unregister(m_hTimer);
					m_hTimer = NULL;
				}
				m_hTimer = TinyApplication::GetInstance()->GetTimers().Register(&TinyVisualAnimation::OnTimer, this, 40, 1);
				return m_hTimer != NULL;
			}
			return FALSE;
		}

		VOID CALLBACK TinyVisualAnimation::OnTimer(PVOID lpParam, BOOLEAN TimerOrWaitFired)
		{
			TinyVisualAnimation* spvis = static_cast<TinyVisualAnimation*>(lpParam);
			if (spvis->m_hTimer != NULL)
			{
				spvis->m_index = ((spvis->m_index + 1) == spvis->m_images.GetSize()) ? 0 : (spvis->m_index + 1);
				TinyApplication::GetInstance()->GetTimers().Change(spvis->m_hTimer, 40, 1);
				spvis->Invalidate();
			}
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
			ASSERT(m_document);
			TinyClipCanvas canvas(hDC, this, rcPaint);
			TinyRectangle clip = m_document->GetWindowRect(this);
			ASSERT(m_index >= 0 || m_index < m_images.GetSize());
			TinyRectangle srcRect = m_images[m_index]->GetRectangle();
			canvas.DrawBitmap(m_images[m_index]->GetHBITMAP(), clip, srcRect);
			return TRUE;
		}
	}
}
