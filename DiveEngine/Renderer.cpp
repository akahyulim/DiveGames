#include "Renderer.h"
#include "Log.h"
#include <assert.h>

namespace Dive
{
	Renderer::~Renderer()
	{
		// ���ҽ� ����
	}

	void Renderer::Initialize()
	{
		if (!m_device || !m_device->IsInitialized())
		{
			CORE_ERROR("Graphics Device�� �������� �ʾ� �ʱ�ȭ�� ������ �� �����ϴ�. ���α׷��� �����մϴ�.");
			PostQuitMessage(0);
		}

		createSamplers();
		createDepthStencilStates();
		createConstantBuffers();
		createTextures();
		createShaders();
		// ��� Resource�� ������ �� PipelineState�� �����.
		// ��� states, shaders, inputlayouts�� ������� �־�� �Ѵ�.
		
		CORE_TRACE("Renderer �ʱ�ȭ�� �����Ͽ����ϴ�.");
	}

	void Renderer::SetDevice(std::shared_ptr<GraphicsDevice> device)
	{
		m_device = device;

		assert(m_device);

		// �� wicked�� ���⿡�� sampler�� �����ұ�?
	}

	void Renderer::createSamplers()
	{
		// ��� ������ ���ΰ�?
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

		// state�� �̷��� �Ѵ� �ص� buffer ������ �ٸ��� �Ѵٸ�....?
		// �ϰ����� �����ϴ°� �����ٵ�...
	}

	void Renderer::createConstantBuffers()
	{
	}

	void Renderer::createTextures()
	{
		// �ؽ����� ��� �������� �� �ִ�.
		// �ϴ� view ���� ����� �ٸ��� sub resource���� �����ؾ� �Ѵ�.
		// �׸��� �����ϴ� texture�� ������ �� ���� �� ����.
	}

	void Renderer::createShaders()
	{
		// Input Layout�� �Բ� ����
	}

	void Renderer::createPipelineStates()
	{
		// ������ shaders, states, inputLayout, prmitive_topology��
		// PipelineStateDesc�� ������ ��
		// GraphicsDevice::CreateRenderPipelineState()�� �����Ͽ�
		// PipelineState�� �����Ѵ�.
		// ��� state, inputLayout���� �� ������ �����ߴٸ�
		// CreateRenderPipelineState()�� Graphics�� ���� �� ������ �ʿ䰡 ����.
		// �׸��� PipelineStateDesc�� PipelineState�� ���е� �ǹ̰� ����.

		// PipelineState�� Draw �������� ����� State�� Sahder�� �����̴�.
		// �̿��� Buffer�� ���� bind�Ͽ��� �Ѵ�.
	}
}