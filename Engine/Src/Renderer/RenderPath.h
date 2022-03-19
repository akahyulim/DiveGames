#pragma once

namespace Dive
{
	class CommandList;

	class RenderPath
	{
	public:
		virtual ~RenderPath() = default;

		virtual void Update(float delta);
		virtual void Render();

	private:
		void passDefault(CommandList* pCl);

	private:
	};
}