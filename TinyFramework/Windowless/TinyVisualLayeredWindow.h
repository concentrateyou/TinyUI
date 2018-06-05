#pragma once
#include "TinyVisual.h"
#include "TinyVisualWindowless.h"

namespace TinyFramework
{
	namespace Windowless
	{
		/// <summary>
		/// ·Ö²ã´°¿Ú
		/// </summary>
		class TinyVisualLayeredWindow : public TinyVisualWindowless
		{
			DISALLOW_COPY_AND_ASSIGN(TinyVisualLayeredWindow)
		public:
			TinyVisualLayeredWindow();
			virtual ~TinyVisualLayeredWindow();
			LPCSTR	RetrieveClassName() OVERRIDE;
			LPCSTR	RetrieveTitle() OVERRIDE;
			HICON	RetrieveIcon() OVERRIDE;
			DWORD	RetrieveStyle() OVERRIDE;
			DWORD	RetrieveExStyle() OVERRIDE;
		public:
			void	OnInitialize() OVERRIDE;
			void	OnUninitialize() OVERRIDE;
		public:
			BOOL	Draw();
		};
	}
}



