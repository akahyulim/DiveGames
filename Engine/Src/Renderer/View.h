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
	struct Batch;
	struct BatchQueue;

	enum class eRenderPath;

	struct ScenePassInfo
	{
		unsigned int m_PassIndex = 0;
		bool m_bAllowInstancing = false;
		bool m_bMarkStencil = false;
		bool m_bVertexLight = false;
		BatchQueue* m_pBatchQueue = nullptr;
	};

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

		std::vector<ScenePassInfo> m_ScenePasses;

		std::vector<Drawable*> m_Drawables;

		// 이렇게 관리하면 안될 듯 하다. pass index와 BatchQueue로 관리해야 한다.
		std::vector<Batch> m_BaseBatchRenderers;
	};
}