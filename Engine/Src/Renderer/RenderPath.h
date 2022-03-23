#pragma once
#include "Renderer.h"

namespace Dive
{
	class GameObject;
	class CommandList;

	class RenderPath
	{
	public:
		RenderPath();
		virtual ~RenderPath();

		virtual void Update(float delta);
		virtual void Render();

	
	protected:
		void passDefault(CommandList* pCl, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

	protected:
		Visibility m_MainVisibilities;
	};
}