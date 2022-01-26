#pragma once
#include "Resource/Resource.h"
#include "Base/DiveCore.h"
#include <d3d11_3.h>
#include <DXGI1_3.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

namespace Dive
{
	class dvTexture : public Resource
	{
	public:
		dvTexture()
		{
		}
		virtual ~dvTexture() = default;

		// get & set
		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

	protected:
		// ansioLevel
		// mipMapBias
		// filter mode
		// wrap mode
		unsigned int m_Width = 0;
		unsigned int m_Height = 0;
	};
}