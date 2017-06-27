#pragma once
#include "MImageController.h"
using namespace DXFramework;

namespace MShow
{
	/// <summary>
	/// Í¼Æ¬ÔªËØ
	/// </summary>
	class MImageElement : public DX11Image2D
	{
		DECLARE_DYNAMIC(MImageElement)
	public:
		MImageElement(MImageController& controller);
		virtual ~MImageElement();
	public:
		BOOL Allocate(DX11& dx11) OVERRIDE;
		BOOL Process(DX11& dx11) OVERRIDE;
		void Deallocate(DX11& dx11) OVERRIDE;
	private:
		MImageController&	m_controller;
	};
}


