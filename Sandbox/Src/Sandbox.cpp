#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox(Dive::Context* pContext)
		: Dive::Application(pContext),
		m_pScene(nullptr)
	{
	}

	Sandbox::~Sandbox()
	{
	}

	// ���� ���� ���� ȣ��
	void Sandbox::Setup()
	{
		// ini ���� ���� �о���� ���� ����.
		m_EngineParameters.title = L"Sandbox";
		m_EngineParameters.width = 800;
		m_EngineParameters.height = 600;
		m_EngineParameters.positionX = m_EngineParameters.GetCenterPositionX();
		m_EngineParameters.positionY = m_EngineParameters.GetCenterPositionY();
		m_EngineParameters.bReSizable = true;//false;

		// resource paths?
	}

	// ���� �ʱ�ȭ �� ȣ��
	void Sandbox::Start()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Start ȣ��");

		// create scene content
		// scene�� ���� ������ �� cache�� �̿��Ͽ� object�� ����
		m_pScene = new Dive::Scene(m_pContext);
		
		// test
		{
			Dive::Shader vs(m_pContext, Dive::eShaderType::Vertex);
			vs.Compile("../CoreData/Shaders/test.hlsl");

			Dive::Shader ps(m_pContext, Dive::eShaderType::Pixel);
			ps.Compile("../CoreData/Shaders/test.hlsl");

			auto pDeviceContext = GetSubsystem<Dive::Graphics>()->GetDeviceContext();
			pDeviceContext->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(vs.GetShader()), NULL, 0);
			pDeviceContext->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(ps.GetShader()), NULL, 0);
		}

		// camera�� ��� ������ ���� ����

		// create ui content

		// setup viewport
		// 1. viewport�� ������ �� renderer�� set
		// viewport�� scene, camera �׸��� �ʿ��ϴٸ� render path�� �����Ͽ� ����
		// 2. render path�� ���ٸ� cache�κ��� load�� �� viewport�� set
		auto pView = std::make_shared<Dive::View>(m_pContext);
		GetSubsystem<Dive::Renderer>()->SetView(0, pView);

		// subscirbe events
		// update handle�Լ����� camera move �Լ� ȣ��
		// post render update handle �Լ����� debug geometry ȣ��
	}

	void Sandbox::Stop()
	{
		DV_LOG_CLIENT_DEBUG("Sandbox::Stop ȣ��");

		// Application Cleanup
		DV_DELETE(m_pScene);
	}
}