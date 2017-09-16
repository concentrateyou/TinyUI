#pragma once
#include "MVideoController.h"
using namespace DXFramework;

namespace MShow
{
	/// <summary>
	/// ÊÓÆµÔªËØ
	/// </summary>
	class MVideoElement : public DX11Image2D
	{
		friend class MPreviewController;
		DECLARE_DYNAMIC(MVideoElement)
	public:
		MVideoElement(MVideoController& controller);
		virtual ~MVideoElement();
	public:
		MVideoController& GetController();
		BOOL Allocate(DX11& dx11) OVERRIDE;
		BOOL Process(DX11& dx11) OVERRIDE;
		void Deallocate(DX11& dx11) OVERRIDE;
	private:
		MVideoController&	m_controller;
	};
}


