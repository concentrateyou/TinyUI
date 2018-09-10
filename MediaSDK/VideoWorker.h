#pragma once
#include "MediaSDK.h"
#include "CameraVisual2D.h"
#include "GameVisual2D.h"
#include "MonitorVisual2D.h"
#include "StreamVisual2D.h"
#include "ImageVisual2D.h"

#define NUM_TEXTURES 2

namespace MediaSDK
{
	class VideoWorker
	{
		DISALLOW_COPY_AND_ASSIGN(VideoWorker)
	public:
		VideoWorker(TinyTaskManeger& manager);
		~VideoWorker();
		DX11&		GetDX11();
		BOOL		Initialize(HWND hWND, const TinySize& canvasSize, const TinySize& outputSize);
		BOOL		Resize(const TinySize& size);
		BOOL		Start();
		void		Stop();
	public:
		void		Add(IVisual2D* visual);
		void		Remove(IVisual2D* visual);
		void		RemoveAll();
		void		MoveUp(IVisual2D* visual);
		void		MoveDown(IVisual2D* visual);
		IVisual2D*  HitTest(const TinyPoint& pos);
	private:
		void		OnMessagePump();
		void		OnDisplay();
		void		OnOutput();
		void		OnSleep(UINT64& time, UINT64 intervalNS);
	private:
		BOOL						m_stop;
		DOUBLE                      m_videoFPS;//ʵʱFPS
		UINT32						m_texture;
		TinyLock					m_lock;
		DX11						m_dx11;
		DX11Graphics2D				m_display;
		DX11Graphics2D*				m_videos[NUM_TEXTURES];
		TinyArray<IVisual2D*>		m_visuals;
		TinyTaskManeger::TinyTask*  m_videoTask;
		TinyTaskManeger&			m_manager;
	};
}

