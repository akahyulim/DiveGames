#pragma once
#include "Core/Object.h"

namespace Dive 
{
	class Graphics;
	class Renderer;
	class Scene;
	class Texture;
	class Viewport;
	class Mesh;
	class Drawable;
	struct BatchRenderer;

	enum class eRenderPath;

	class View : public Object
	{
		DIVE_OBJECT(View, Object)

	public:
		explicit View(Context* pContext);
		~View() override;

		void Update(float delta);
		void Render();

		bool Define(Texture* pRenderTarget, Viewport* pViewport);

		Graphics* GetGraphics() const { return m_pGraphics; }

	private:
		void getDrawables();
		void getBaseBatches();

	private:
		Graphics* m_pGraphics = nullptr;
		Renderer* m_pRenderer = nullptr;
		Scene* m_pScene = nullptr;

		Texture* m_pRenderTarget = nullptr;
		Texture* m_pCurrentRenderTarget = nullptr;

		RECT m_ViewRect = { 0, 0, 0, 0 };
		DirectX::XMINT2 m_ViewSize = { 0 ,0 };
		DirectX::XMINT2 m_RenderTargetSize = { 0, 0 };

		std::vector<Drawable*> m_Drawables;
		std::vector<BatchRenderer> m_BaseBatchRenderers;
	};
}