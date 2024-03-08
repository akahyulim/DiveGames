#pragma once
#include "RendererDefs.h"		// eRenderPath용. 추후 Renderer에 흡수시키자.
#include "Renderer.h"

namespace Dive
{
	class Scene;
	class Camera;
	class Renderable;
	class Light;
	class Mesh;
	class Material;

	// urho처럼 하려면 이게 batch와 같이 함수를 지닌 클래스여야 한다.
	struct RenderableBatch
	{
		// camera distance
		DirectX::XMMATRIX worldTransform;
		Mesh* pMesh;
		// mesh type
		Material* pMaterial;
	};

	struct LightBatch
	{

	};

	// 아무래도 이건 유니티 혹은 sparky의 Layer가 어울린다.
	// 추후 더 생각해보자.
	class ViewScreen
	{
	public:
		ViewScreen(Camera* pCamera, eRenderPath renderPath = eRenderPath::Forward);

		void Update();
		void Render();

	private:
		void forwardRender();
		void deferredRender();

	private:
		Scene* m_pActiveScene;
		Camera* m_pCamera;
		eRenderPath m_RenderPath;

		std::vector<Renderable*> m_Renderables;
		std::vector<Light*> m_Lights;

		std::vector<RenderableBatch> m_RenderableBatches;
		std::vector<LightBatch> m_LightBatches;

		FrameBuffer m_cpuFrameBuffer;
		MaterialBuffer m_cpuMaterialBuffer;
		CameraBuffer m_cpuCameraBuffer;
		LightBuffer m_cpuLightBuffer;
	};
}