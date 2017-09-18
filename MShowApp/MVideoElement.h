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
		void SetPush(BOOL bVal);
		BOOL IsPush() const;
	public:
		MVideoController& GetController();
		BOOL Allocate(DX11& dx11) OVERRIDE;
		BOOL Process(DX11& dx11) OVERRIDE;
		void Deallocate(DX11& dx11) OVERRIDE;
	private:
		BOOL				m_bPush;
		MVideoController&	m_controller;
	};
}


