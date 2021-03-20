#pragma once
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include <memory>

namespace Dive
{
	// visibility
	// Draw ����� �����ϴ� Ŭ�����̴�.
	// RenderPath3D���� �����ϰ� scene, camera���� ���� ��
	// Renderer::UpdateVisibility()�� ���� �� ������ JobSystem���� ����� �߷�����.
	// �̷��� �����Ǵ� ������ ���߿� Draw�� ���޵ȴ�.

	// PipelineState
	// States, Shaders, InputLayout�� ������ ��ü
	// �̸� Graphics�� �Ѱ� �� ���� bind�Ѵ�.

	class Renderer
	{
	public:
		static Renderer& GetInstance()
		{
			static Renderer instance;
			return instance;
		}

		void Initialize();

		// update�� draw�� �ִ�.

		GraphicsDevice* GetDevice() { return m_device.get(); }
		void SetDevice(std::shared_ptr<GraphicsDevice> device);

	private:
		Renderer() = default;
		~Renderer();

		// ���� GPU Resource ����
		void createSamplers();
		void createDepthStencilStates();
		void createConstantBuffers();
		void createTextures();
		void createShaders();
		void createPipelineStates();

	private:
		// ����... ���ʿ��� �̸��� �ٲ�� �ϳ�...
		std::shared_ptr<GraphicsDevice> m_device;

		// GPU Resource ����
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState[DSSTATE_COUNT];
	};
}