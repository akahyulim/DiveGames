#pragma once
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include <memory>

namespace Dive
{
	enum class eVisibilityType
	{
		Invalid = 0,
		Objects,
		Lights,
	};

	class Visibility
	{
	public:
	private:
		// frustum도 있다.
	};

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
		// update는 visibility, PerFrameData,  RenderData, CameraCB 등이 있다.

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