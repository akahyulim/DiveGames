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
		ScenePassInfo()
			: PassIndex(0),
			bAllowInstancing(false),
			bMarkStencil(false),
			bVertexLight(false),
			pBatchQueue(nullptr)
		{}

		uint32_t PassIndex;
		bool bAllowInstancing;
		bool bMarkStencil;
		bool bVertexLight;
		BatchQueue* pBatchQueue;
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
		Graphics* m_pGraphics;
		Renderer* m_pRenderer;
		Scene* m_pScene;

		Texture* m_pRenderTarget;
		Texture* m_pCurRenderTarget;

		RECT m_ViewRect;
		DirectX::XMINT2 m_ViewSize;
		DirectX::XMINT2 m_RenderTargetSize;

		std::vector<ScenePassInfo> m_ScenePasses;

		std::vector<Drawable*> m_Drawables;

		// 이렇게 관리하면 안될 듯 하다. pass index와 BatchQueue로 관리해야 한다.
		std::vector<Batch> m_BaseBatchRenderers;
	};
}