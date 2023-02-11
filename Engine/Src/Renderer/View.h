#pragma once
#include "Core/Object.h"

namespace Dive 
{
	class Graphics;
	class Renderer;
	class Camera;
	class Scene;
	class RenderPath;
	struct RenderPathCommand;
	class Texture;
	class Viewport;
	class Mesh;
	class Drawable;
	class Light;
	class Technique;
	class Batch;
	class BatchQueue;

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
		ScenePassInfo(const ScenePassInfo& rhs) = default;

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
		Renderer* GetRenderer() const { return m_pRenderer; }

	private:
		void getDrawables();
		void getBaseBatches();

		void addBatchToQueue(BatchQueue& queue, Batch& batch, Technique* pTech, bool bAllowInstancing);

		void executeRenderPathCommands();

		void setRenderTargets(RenderPathCommand* pCommand);
		void setTextures(RenderPathCommand* pCommand);

		Texture* findNamedTexture(const std::string& name, bool bRenderTarget);

	private:
		Graphics* m_pGraphics;
		Renderer* m_pRenderer;
		Scene* m_pScene;

		Camera* m_pCamera;

		RenderPath* m_pRenderPath;

		Texture* m_pRenderTarget;
		Texture* m_pCurRenderTarget;

		RECT m_ViewRect;
		DirectX::XMINT2 m_ViewSize;
		DirectX::XMINT2 m_RenderTargetSize;

		std::vector<Drawable*> m_Drawables;
		std::vector<Light*> m_Lights;

		std::vector<ScenePassInfo> m_ScenePasses;
		std::unordered_map<uint32_t, BatchQueue> m_BatchQueues;
	};
}