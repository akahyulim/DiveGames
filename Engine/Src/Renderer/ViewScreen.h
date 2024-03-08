#pragma once
#include "RendererDefs.h"		// eRenderPath��. ���� Renderer�� �����Ű��.
#include "Renderer.h"

namespace Dive
{
	class Scene;
	class Camera;
	class Renderable;
	class Light;
	class Mesh;
	class Material;

	// urhoó�� �Ϸ��� �̰� batch�� ���� �Լ��� ���� Ŭ�������� �Ѵ�.
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

	// �ƹ����� �̰� ����Ƽ Ȥ�� sparky�� Layer�� ��︰��.
	// ���� �� �����غ���.
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