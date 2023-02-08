#include "ForwardLights.h"

DEFINE_APPLICATION_MAIN(ForwardLights)

ForwardLights::ForwardLights(Dive::Context* pContext)
	: Dive::Application(pContext),
	m_pScene(nullptr),
	m_pMainCamera(nullptr)
{
}

ForwardLights::~ForwardLights()
{
	DV_LOG_CLIENT_TRACE("ForwardLights �Ҹ� �Ϸ�");
}

// ���� ���� ���� ȣ��
void ForwardLights::Setup()
{
	// ini ���� ���� �о���� ���� ����.
	m_EngineParams.Title = L"Sample 01_ForwardLights";
	m_EngineParams.Width = 1280;
	m_EngineParams.Height = 760;
	m_EngineParams.PositionX = m_EngineParams.GetCenterPositionX();
	m_EngineParams.PositionY = m_EngineParams.GetCenterPositionY();
	m_EngineParams.bReSizable = false;

	// resource paths?
}

// ���� �ʱ�ȭ �� ȣ��
void ForwardLights::Start()
{
	DV_LOG_CLIENT_TRACE("ForwardLights::Start ȣ��");

	createScene();

	// urho�� subscribeToEvents()��� �޼��带 ���� �Ʒ� ������ ����
	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
}

void ForwardLights::Stop()
{
	DV_LOG_CLIENT_TRACE("ForwardLights::Stop ȣ��");

	DV_DELETE(m_pScene);
}

void ForwardLights::OnUpdate(const Dive::Event& evnt)
{
	// �̺�Ʈ���� delta�� ��� �����ؾ� �Ѵ�.
	moveCamera(1.0f);
}

void ForwardLights::createScene()
{
	auto pRenderer = GetSubsystem<Dive::Renderer>();
	auto pCache = GetSubsystem<Dive::ResourceCache>();

	// Create Scene
	m_pScene = new Dive::Scene(m_pContext);
	m_pScene->SetName("ForwardLights");
	auto pViewport = new Dive::Viewport(m_pContext);
	pViewport->SetScene(m_pScene);
	pRenderer->SetViewport(0, pViewport);
	GetSubsystem<Dive::Graphics>()->SetWindowTitle(Dive::FileSystem::StringToWstring(m_pScene->GetName()));

	// Add Main Camera
	m_pMainCamera = m_pScene->CreateGameObject("MainCamera");
	auto pCamera = m_pMainCamera->AddComponent<Dive::Camera>();
	auto pTransform = m_pMainCamera->GetComponent<Dive::Transform>();
	pTransform->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f));
	pViewport->SetCamera(pCamera);

	// Add GameObject
	{
		// Sphere
		{
			auto pSphereObject = m_pScene->CreateGameObject("RedSphere");
			auto pTransform = pSphereObject->GetComponent<Dive::Transform>();
			// ���� ��ġ�� ���� ������ ���Ҵ�. Drawable::Update()�� �� �� �ִ� ����� ���� �����̴�.
			pTransform->SetPosition(DirectX::XMFLOAT3(-5.0f, 0.0f, 0.0f));
			pTransform->SetScale(2.0f, 2.0f, 2.0f);
			auto pDrawable = pSphereObject->AddComponent<Dive::Drawable>();
			pDrawable->SetModel(getModel("Cube"));
			Dive::Material* pMat = new Dive::Material(m_pContext);
			pMat->SetName("RedSphereMat");
			pCache->AddManualResource(pMat);
			pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
			pMat->SetTexture(Dive::eTextureUnit::Diffuse, Dive::Texture2D::GetRedTexture(m_pContext));
			pDrawable->SetMaterial(pMat);
		}

		// Cube
		{
			auto pCubeObject = m_pScene->CreateGameObject("Cube");
			auto pTransform = pCubeObject->GetComponent<Dive::Transform>();
			// ���� ��ġ�� ���� ������ ���Ҵ�. Drawable::Update()�� �� �� �ִ� ����� ���� �����̴�.
			pTransform->SetPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f));
			pTransform->SetScale(2.0f, 2.0f, 2.0f);
			auto pDrawable = pCubeObject->AddComponent<Dive::Drawable>();
			pDrawable->SetModel(getModel("Cube"));
			Dive::Material* pMat = new Dive::Material(m_pContext);
			pMat->SetName("CubeMat");
			pCache->AddManualResource(pMat);
			pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
			pMat->AddTexture(Dive::eTextureUnit::Diffuse, "Assets/Textures/seafloor.dds");
			pDrawable->SetMaterial(pMat);
		}
	}

	// Add Lights
	{

	}

	DV_LOG_CLIENT_TRACE("ForwardLights Scene�� �����Ͽ����ϴ�.");
}

Dive::Model* ForwardLights::getModel(const std::string& name)
{
	auto pCache = GetSubsystem<Dive::ResourceCache>();
	auto pModel = pCache->GetExistingResource<Dive::Model>(name);
	if (pModel)
		return pModel;
	else
	{
		if (name == "Triangle")
		{
			// �ᱹ �̷��� ���´� �Ұ����� �� ���δ�.
			// Geometry Type�� ���� Elements�� �̹� ������ ���¶� ������ �� �ִ�.
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD);
			//elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_NORMAL);

			std::vector<float> vertices =
			{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,// 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.0f,// 0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };//, 0.0f, 0.0f, 1.0f };

			Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
			pVb->SetSize(3, elements);
			pVb->SetData(static_cast<void*>(vertices.data()));

			std::vector<unsigned short> indices = { 0, 1, 2 };
			Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
			pIb->Create<unsigned short>(indices);

			Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
			pMesh->SetVertexBuffer(pVb);
			pMesh->SetIndexBuffer(pIb);

			Dive::Model* pTriangleModel = new Dive::Model(m_pContext);
			pTriangleModel->SetName("TriangleModel");
			pTriangleModel->SetNumMeshes(1);
			pTriangleModel->SetMesh(0, pMesh);
			pCache->AddManualResource(pTriangleModel);

			DV_LOG_CLIENT_TRACE("Triangle Model�� �����Ͽ����ϴ�.");

			return pTriangleModel;
		}
		else if (name == "Quad")
		{
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD);

			std::vector<float> vertices =
			{
				-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
			};

			Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
			pVb->SetSize(4, elements);
			pVb->SetData(static_cast<void*>(vertices.data()));

			std::vector<unsigned short> indices = { 0, 1, 2, 0, 2, 3 };
			Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
			pIb->Create<unsigned short>(indices);

			Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
			pMesh->SetVertexBuffer(pVb);
			pMesh->SetIndexBuffer(pIb);

			Dive::Model* pQuadModel = new Dive::Model(m_pContext);
			pQuadModel->SetName("QuadModel");
			pQuadModel->SetNumMeshes(1);
			pQuadModel->SetMesh(0, pMesh);
			pCache->AddManualResource(pQuadModel);

			DV_LOG_CLIENT_TRACE("Quad Model�� �����Ͽ����ϴ�.");

			return pQuadModel;
		}
		else if (name == "Cube")
		{
			std::vector<Dive::VertexElement> elements;
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
			elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD);

			std::vector<float> vertices =
			{
				// front
				-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

				// bottom
				-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

				// back
				-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

				// top
				-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

				// right
				1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,

				// left
				-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
			};

			Dive::VertexBuffer* pVb = new Dive::VertexBuffer(m_pContext);
			pVb->SetSize(24, elements);
			pVb->SetData(static_cast<void*>(vertices.data()));

			std::vector<unsigned short> indices = {
				0, 2, 3, 0, 1, 2,
				4, 6, 7, 4, 5, 6,
				9, 11, 10, 9, 8, 11,
				12, 14, 15, 12, 13, 14,
				16, 18, 19, 16, 17, 18,
				20, 22, 23, 20, 21, 22
			};
			Dive::IndexBuffer* pIb = new Dive::IndexBuffer(m_pContext);
			pIb->Create<unsigned short>(indices);

			Dive::Mesh* pMesh = new Dive::Mesh(m_pContext);
			pMesh->SetVertexBuffer(pVb);
			pMesh->SetIndexBuffer(pIb);

			Dive::Model* pCubeModel = new Dive::Model(m_pContext);
			pCubeModel->SetName("CubeModel");
			pCubeModel->SetNumMeshes(1);
			pCubeModel->SetMesh(0, pMesh);
			pCache->AddManualResource(pCubeModel);

			DV_LOG_CLIENT_TRACE("Cube Model�� �����Ͽ����ϴ�.");

			return pCubeModel;
		}
		else if (name == "Sphere")
		{
		}
	}

	return nullptr;
}

Dive::Technique* ForwardLights::getTechnique(const std::string& name)
{
	auto pCache = GetSubsystem<Dive::ResourceCache>();
	Dive::Technique* pTechnique = pCache->GetExistingResource<Dive::Technique>(name);
	if (pTechnique)
		return pTechnique;
	else
	{
		if (name == "BasicVColorUnlitAlpha")
		{
			pTechnique = new Dive::Technique(m_pContext);
			pTechnique->SetName("BasicVColorUnlitAlpha");
			Dive::Pass* pPass = pTechnique->CreatePass("alpha");
			// global�� pass���� �����ϴ� �� �ϴ�.
			// ���� ���̴��� �ִٸ� �׶��� ���� �����Ѵ�.
			pPass->SetVertexShader("CoreData/Shaders/Basic.hlsl");
			//pPass->SetVertexShaderDefines("VERTEXCOLOR DIFFMAP");
			pPass->SetVertexShaderDefines("DIFFMAP");
			pPass->SetPixelShader("CoreData/Shaders/Basic.hlsl");
			// ���� ����� �ٸ���. ������ ����� �Ѵ�.
			//pPass->SetPixelShaderDefines("VERTEXCOLOR DIFFMAP");
			pPass->SetPixelShaderDefines("DIFFMAP");
			//pPass->SetPixelShaderDefines("VERTEXCOLOR");
			// depthwrite = false
			// blend = alpha
			pCache->AddManualResource(pTechnique);

			return pTechnique;
		}
	}

	return nullptr;
}

void ForwardLights::moveCamera(float delta)
{
	static float MOVE_SPEED = 10.0f;
	static DirectX::XMVECTOR FORWARD = { 0.0f, 0.0f, 1.0f };
	static DirectX::XMVECTOR BACK = { 0.0f, 0.0f, -1.0f };
	static DirectX::XMVECTOR LEFT = { -1.0f, 0.0f, 0.0f };
	static DirectX::XMVECTOR RIGHT = { 1.0f, 0.0f, 0.0f };
	static DirectX::XMVECTOR UP = { 0.0f, 1.0f, 0.0f };
	static DirectX::XMVECTOR DOWN = { 0.0f, -1.0f, 0.0f };

	auto pInput = GetSubsystem<Dive::Input>();
	if (pInput)
	{
		auto pTransform = m_pMainCamera->GetComponent<Dive::Transform>();

		// xmfloat3�� xmvector ���� speed�� delta�� ���� ���� ����.
		if (pInput->KeyPress(DIK_W))
		{
			pTransform->TranslateVector(FORWARD);
		}
		if (pInput->KeyPress(DIK_S))
		{
			pTransform->Translate(DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f));
		}
		if (pInput->KeyPress(DIK_A))
		{
			pTransform->Translate(DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), Dive::eSpace::World);
		}
		if (pInput->KeyPress(DIK_D))
		{
			pTransform->Translate(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f));
		}
		if (pInput->KeyPress(DIK_C))
		{
			pTransform->Translate(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f));
		}
		if (pInput->KeyPress(DIK_SPACE))
		{
			pTransform->Translate(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		}

		if (pInput->KeyPress(DIK_LEFT))
		{
			pTransform->Rotate(DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f));
		}
		if (pInput->KeyPress(DIK_RIGHT))
		{
			pTransform->Rotate(DirectX::XMFLOAT3(0.0f, 2.0f, 0.0f));
		}
		if (pInput->KeyPress(DIK_UP))
		{
			pTransform->Rotate(DirectX::XMFLOAT3(-2.0f, 0.0f, 0.0f));
		}
		if (pInput->KeyPress(DIK_DOWN))
		{
			pTransform->Rotate(DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
		}
	}
}