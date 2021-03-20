#include "Renderer.h"
#include "Log.h"
#include <assert.h>

namespace Dive
{
	Renderer::~Renderer()
	{
		// 리소스 제거
	}

	void Renderer::Initialize()
	{
		if (!m_device || !m_device->IsInitialized())
		{
			CORE_ERROR("Graphics Device가 생성되지 않아 초기화를 실행할 수 없습니다. 프로그램을 종료합니다.");
			PostQuitMessage(0);
		}

		createSamplers();
		createDepthStencilStates();
		createConstantBuffers();
		createTextures();
		createShaders();
		// 모든 Resource가 생성된 후 PipelineState를 만든다.
		// 적어도 states, shaders, inputlayouts는 만들어져 있어야 한다.
		
		CORE_TRACE("Renderer 초기화에 성공하였습니다.");
	}

	void Renderer::SetDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_device = device;

		assert(m_device);

		// 왜 wicked는 여기에서 sampler를 생성할까?
	}

	void Renderer::createSamplers()
	{
		// 어떻게 생성할 것인가?
	}

	void Renderer::createDepthStencilStates()
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable	= true;
		desc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc		= D3D11_COMPARISON_GREATER;

		if (FAILED(m_device->GetDevice()->CreateDepthStencilState(&desc, &m_depthStencilState[DSSTATE_DEFAULT])))
		{
			CORE_ERROR("");
		}

		// state는 이렇게 한다 해도 buffer 생성은 다르게 한다면....?
		// 일관성을 유지하는게 나을텐데...
	}

	void Renderer::createConstantBuffers()
	{
	}

	void Renderer::createTextures()
	{
		// 텍스쳐의 경우 복잡해질 수 있다.
		// 일단 view 생성 요건이 다르고 sub resource까지 설정해야 한다.
		// 그리고 생성하는 texture의 개수도 꽤 많을 것 같다.
	}

	void Renderer::createShaders()
	{
		// Input Layout도 함께 생성
	}

	void Renderer::createPipelineStates()
	{
		// 생성한 shaders, states, inputLayout, prmitive_topology로
		// PipelineStateDesc를 구성한 후
		// GraphicsDevice::CreateRenderPipelineState()에 전달하여
		// PipelineState를 생성한다.
		// 사실 state, inputLayout까지 이 곳에서 생성했다면
		// CreateRenderPipelineState()를 Graphics에 선언 및 정의할 필요가 없다.
		// 그리고 PipelineStateDesc와 PipelineState의 구분도 의미가 없다.

		// PipelineState는 Draw 단위에서 사용할 State와 Sahder의 묶음이다.
		// 이외의 Buffer는 따로 bind하여야 한다.
	}
}