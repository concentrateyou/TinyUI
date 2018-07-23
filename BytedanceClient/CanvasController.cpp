#include "stdafx.h"
#include "CanvasController.h"

namespace Bytedance
{
	//CanvasController::CanvasController(CanvasView& view)
	//	:m_view(view),
	//	m_displayCanvas(m_dx11),
	//	m_videoCanvas(m_dx11),
	//	m_bClose(FALSE)
	//{

	//}

	//CanvasController::~CanvasController()
	//{
	//}
	//DX11& CanvasController::GetDX11()
	//{
	//	return m_dx11;
	//}
	//BOOL CanvasController::Initialize(const TinySize& size)
	//{
	//	TinyRectangle s;
	//	GetClientRect(m_view.Handle(), &s);
	//	if (!m_dx11.Initialize(m_view.Handle(), TO_CX(s), TO_CY(s)))
	//		return FALSE;
	//	if (!m_displayCanvas.Create())
	//		return FALSE;
	//	if (!m_displayCanvas.InitializeShaders())
	//		return FALSE;
	//	if (!m_videoCanvas.Create(size.cx, size.cy))
	//		return FALSE;
	//	if (!m_videoCanvas.InitializeShaders())
	//		return FALSE;
	//	m_bClose = FALSE;
	//	m_tasks.Initialize(1, 5);
	//	m_woker.Reset(new TinyWin32Worker(&m_tasks));
	//	ASSERT(m_woker);
	//	m_woker->Submit(BindCallback(&CanvasController::OnMessagePump, this));
	//	return TRUE;
	//}
	//void CanvasController::Uninitialize()
	//{
	//	m_bClose = TRUE;
	//	m_woker->Wait(TRUE);
	//	m_woker->Close();
	//	m_tasks.Cancel();
	//	m_tasks.Close();
	//	m_displayCanvas.Destory();
	//	m_videoCanvas.Destory();
	//	RemoveAll();
	//}
	//void CanvasController::OnMessagePump()
	//{
	//	for (;;)
	//	{
	//		if (m_bClose)
	//			break;
	//		DisplayRender();
	//		VideoRender();
	//		Sleep(30);
	//	}
	//}
	//void CanvasController::DisplayRender()
	//{
	//	/*TinyAutoLock lock(m_lock);
	//	m_displayCanvas.BeginDraw();
	//	for (INT i = 0; i < m_visuals.GetSize(); i++)
	//	{
	//		IVisual* visual = m_visuals[i];
	//		if (visual->Process())
	//		{
	//			if (visual->GetVisual()->IsKindOf(RUNTIME_CLASS(DX11Image2D)))
	//			{
	//				m_videoCanvas.DrawImage(*static_cast<DX11Image2D*>(visual->GetVisual()));
	//			}
	//			if (visual->GetVisual()->IsKindOf(RUNTIME_CLASS(DX11YUVVideo)))
	//			{
	//				m_videoCanvas.DrawImageYUVBT601(*static_cast<DX11YUVVideo*>(visual->GetVisual()));
	//			}
	//			if (visual->GetVisual()->IsKindOf(RUNTIME_CLASS(DX11YUY2Video)))
	//			{
	//				m_videoCanvas.DrawImageYUY2BT601(*static_cast<DX11YUY2Video*>(visual->GetVisual()));
	//			}
	//		}
	//	}
	//	m_displayCanvas.EndDraw();
	//	m_dx11.Present();*/
	//}
	//void CanvasController::VideoRender()
	//{

	//}
	///*void CanvasController::Add(IVisual* ps)
	//{
	//	TinyAutoLock lock(m_lock);
	//	for (INT i = 0; i < m_visuals.GetSize(); i++)
	//	{
	//		IVisual* visual = m_visuals[i];
	//		if (strcasecmp(visual->GetName(), ps->GetName()) == 0)
	//		{
	//			m_visuals.Remove(visual);
	//			SAFE_DELETE(visual);
	//			break;
	//		}
	//	}
	//	m_visuals.Add(ps);
	//}
	//void CanvasController::Remove(IVisual* ps)
	//{
	//	TinyAutoLock lock(m_lock);
	//	if (ps != NULL)
	//	{
	//		ps->Close();
	//	}
	//	m_visuals.Remove(ps);
	//}
	//void CanvasController::RemoveAll()
	//{
	//	TinyAutoLock lock(m_lock);
	//	for (INT i = 0; i < m_visuals.GetSize(); i++)
	//	{
	//		m_visuals[i]->Close();
	//		SAFE_DELETE(m_visuals[i]);
	//	}
	//	m_visuals.RemoveAll();
	//}*/
}
