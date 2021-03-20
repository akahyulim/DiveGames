#pragma once
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include <memory>

namespace Dive
{
	// visibility
	// Draw 대상을 관리하는 클래스이다.
	// RenderPath3D에서 관리하고 scene, camera등을 받은 후
	// Renderer::UpdateVisibility()를 통해 매 프레임 JobSystem으로 대상을 추려낸다.
	// 이렇게 관리되는 대상들은 나중에 Draw에 전달된다.

	// PipelineState
	// States, Shaders, InputLayout을 설정한 객체
	// 이를 Graphics에 넘겨 한 번에 bind한다.

	class Renderer
	{
	public:
		static Renderer& GetInstance()
		{
			static Renderer instance;
			return instance;
		}

		void Initialize();

		// update와 draw가 있다.

		GraphicsDevice* GetDevice() { return m_device.get(); }
		void SetDevice(std::shared_ptr<GraphicsDevice> device);

	private:
		Renderer() = default;
		~Renderer();

		// 각종 GPU Resource 생성
		void createSamplers();
		void createDepthStencilStates();
		void createConstantBuffers();
		void createTextures();
		void createShaders();
		void createPipelineStates();

	private:
		// 흐음... 이쪽에서 이름을 바꿔야 하나...
		std::shared_ptr<GraphicsDevice> m_device;

		// GPU Resource 관리
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState[DSSTATE_COUNT];
	};
}