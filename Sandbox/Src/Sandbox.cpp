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
			// vertices, indices ����
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			//elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_NORMAL);
			//elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD);
			//elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD, 1);

			Dive::VertexBuffer vb(GetContext());
			vb.CreateBuffer(3, elements);

			std::vector<DirectX::XMFLOAT3> triangle;
			triangle.emplace_back(-0.5f, -0.5f, 0.0f);
			triangle.emplace_back(0.5f, -0.5f, 0.0f);
			triangle.emplace_back(0.5f, 0.5f, 0.0f);

			vb.SetData(triangle.data());

			Dive::IndexBuffer ib(GetContext());
			ib.CreateBuffer(3, false);

			std::vector<unsigned int> indices;
			indices.emplace_back(0);
			indices.emplace_back(1);
			indices.emplace_back(2);
			ib.SetData(indices.data());
			

			// shader ����
			Dive::Shader vs(m_pContext, Dive::eShaderType::Vertex);
			vs.Compile("../CoreData/Shaders/test.hlsl");
			Dive::Shader ps(m_pContext, Dive::eShaderType::Pixel);
			ps.Compile("../CoreData/Shaders/test.hlsl");

			// input layout ����
			Dive::InputLayout il(m_pContext, &vb, &vs);

			// buffer bind
			auto pDeviceContext = GetSubsystem<Dive::Graphics>()->GetDeviceContext();
			
			auto* pVB = vb.GetBuffer();
			UINT stride = vb.GetVertexStride();
			UINT offset = 0;
			pDeviceContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
			pDeviceContext->IASetIndexBuffer(ib.GetBuffer(), ib.GetFormat(), 0);

			// shader bind
			pDeviceContext->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(vs.GetShader()), NULL, 0);
			pDeviceContext->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(ps.GetShader()), NULL, 0);

			// inputlayout bind
			pDeviceContext->IASetInputLayout(il.GetInputLayout());

			// destroy
			vb.Release();
			ib.Release();
			vs.Release();
			ps.Release();
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