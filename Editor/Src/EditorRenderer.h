#pragma once

namespace Dive
{
	class EditorRenderer : public Renderer
	{
		DIVE_OBJECT(EditorRenderer, Renderer);

	public:
		EditorRenderer(Context* context);
		~EditorRenderer();

		void Initialize() override;
		void Render() override;

		void ResizeRenderTarget(unsigned int width, unsigned int height);

		Dive_Texture* GetRenderTarget() { return m_EditorViewRenderTarget; }


	private:

	private:
		Dive_Texture* m_EditorViewRenderTarget;
	};
}
