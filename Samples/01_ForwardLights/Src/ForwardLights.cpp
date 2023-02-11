#include "ForwardLights.h"

DEFINE_APPLICATION_MAIN(ForwardLights)

ForwardLights::ForwardLights(Dive::Context* pContext)
	: Dive::Application(pContext),
	m_pScene(nullptr),
	m_pMainCamera(nullptr),
	m_pCubeObject(nullptr),
	m_pTriangleObject(nullptr)
{
}

ForwardLights::~ForwardLights()
{
	DV_LOG_CLIENT_TRACE("ForwardLights 소멸 완료");
}

// 엔진 생성 전에 호출
void ForwardLights::Setup()
{
	// ini 파일 등을 읽어오는 편이 낫다.
	m_EngineParams.Title = L"Sample 01_ForwardLights";
	m_EngineParams.Width = 1280;
	m_EngineParams.Height = 760;
	m_EngineParams.PositionX = m_EngineParams.GetCenterPositionX();
	m_EngineParams.PositionY = m_EngineParams.GetCenterPositionY();
	m_EngineParams.bReSizable = false;

	// resource paths?
}

// 엔진 초기화 후 호출
void ForwardLights::Start()
{
	DV_LOG_CLIENT_TRACE("ForwardLights::Start 호출");

	createScene();

	// urho는 subscribeToEvents()라는 메서드를 통해 아래 내용을 랩핑
	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
}

void ForwardLights::Stop()
{
	DV_LOG_CLIENT_TRACE("ForwardLights::Stop 호출");

	DV_DELETE(m_pScene);
}

void ForwardLights::OnUpdate(const Dive::Event& evnt)
{
	// 이벤트에서 delta를 얻어 전달해야 한다.
	moveCamera(1.0f);

	{
		if (m_pTriangleObject)
		{
			auto pTransform = m_pTriangleObject->GetComponent<Dive::Transform>();
			pTransform->Rotate(0.0f, 1.0f, 0.0f);
		}

		if (m_pCubeObject)
		{
			auto pTransform = m_pCubeObject->GetComponent<Dive::Transform>();
			pTransform->Rotate(0.0f, -1.0f, 0.0f);
		}
	}
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
		// Quad
		{
			auto pQuadObject = m_pScene->CreateGameObject("QuadObject");
			auto pTransform = pQuadObject->GetComponent<Dive::Transform>();
			pTransform->SetPosition(0.0f, -2.0f, 0.0f);
			pTransform->SetScale(5.0f, 5.0f, 1.0f);
			pTransform->SetRotation(90.0f, 0.0f, 0.0f);
			auto pDrawable = pQuadObject->AddComponent<Dive::Drawable>();
			pDrawable->SetModel(getModel("Quad"));
			Dive::Material* pMat = new Dive::Material(m_pContext);
			pMat->SetName("QuadMat");
			pCache->AddManualResource(pMat);
			pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
			pMat->SetTexture(Dive::eTextureUnit::Diffuse, Dive::Texture2D::GetGrayTexture(m_pContext));
			pDrawable->SetMaterial(pMat);
		}

		// Triangle
		{
			m_pTriangleObject = m_pScene->CreateGameObject("TriangleObject");
			auto pTransform = m_pTriangleObject->GetComponent<Dive::Transform>();
			pTransform->SetPosition(0.0f, 0.0f, 0.0f);
			auto pDrawable = m_pTriangleObject->AddComponent<Dive::Drawable>();
			pDrawable->SetModel(getModel("Triangle"));
			Dive::Material* pMat = new Dive::Material(m_pContext);
			pMat->SetName("TriangleMat");
			pCache->AddManualResource(pMat);
			pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
			pMat->SetTexture(Dive::eTextureUnit::Diffuse, Dive::Texture2D::GetWhiteTexture(m_pContext));
			pDrawable->SetMaterial(pMat);
		}

		// Cube
		{
			m_pCubeObject = m_pScene->CreateGameObject("CubeObject");
			auto pTransform = m_pCubeObject->GetComponent<Dive::Transform>();
			pTransform->SetParent(m_pTriangleObject->GetComponent<Dive::Transform>());
			pTransform->SetLocalPosition(5.0f, 0.0f, 0.0f);
			auto pDrawable = m_pCubeObject->AddComponent<Dive::Drawable>();
			pDrawable->SetModel(getModel("Cube"));
			Dive::Material* pMat = new Dive::Material(m_pContext);
			pMat->SetName("CubeMat");
			pCache->AddManualResource(pMat);
			pMat->SetTechnique(getTechnique("BasicVColorUnlitAlpha"));
			pMat->AddTexture(Dive::eTextureUnit::Diffuse, "Assets/Textures/stone01.tga");
			pDrawable->SetMaterial(pMat);
		}
	}

	// Add Lights
	{
		// Directional Light
		{
			auto pDirLightObject = m_pScene->CreateGameObject("DirLightObject");
			auto pTransform = pDirLightObject->GetComponent<Dive::Transform>();
			pTransform->SetPosition(-1.0f, 1.0f, 1.0f);
			auto pLight = pDirLightObject->AddComponent<Dive::Light>();
			pLight->SetType(Dive::eLightType::Directional);
			pLight->SetColor(1.0f, 0.0f, 7.0f);
		}
	}

	DV_LOG_CLIENT_TRACE("ForwardLights Scene을 생성하였습니다.");
}

Dive::Model* ForwardLights::getModel(const std::string& name)
{
	auto pCache = GetSubsystem<Dive::ResourceCache>();
	auto pModel = pCache->GetExistingResource<Dive::Model>(name);
	if (pModel)
		return pModel;
	else
	{
		std::vector<Dive::VertexElement> elements;
		elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_POSITION);
		elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR4, Dive::eVertexElementSemantic::SEM_COLOR);
		elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR2, Dive::eVertexElementSemantic::SEM_TEXCOORD);
		elements.emplace_back(Dive::eVertexElementType::TYPE_VECTOR3, Dive::eVertexElementSemantic::SEM_NORMAL);

		if (name == "Triangle")
		{
			std::vector<float> vertices =
			{ -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f,
			1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f };

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

			DV_LOG_CLIENT_TRACE("Triangle Model을 생성하였습니다.");

			return pTriangleModel;
		}
		else if (name == "Quad")
		{
			std::vector<float> vertices =
			{
				-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
				1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f
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

			DV_LOG_CLIENT_TRACE("Quad Model을 생성하였습니다.");

			return pQuadModel;
		}
		else if (name == "Cube")
		{
			std::vector<float> vertices =
			{
				// front
				-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
				1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
				1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
				-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,

				// bottom
				-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
				1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
				1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
				-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

				// back
				-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
				1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

				// top
				-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
				1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
				-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

				// right
				1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
				1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,

				// left
				-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
				-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f
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

			DV_LOG_CLIENT_TRACE("Cube Model을 생성하였습니다.");

			return pCubeModel;
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
			// global은 pass마다 저장하는 듯 하다.
			// 개별 셰이더가 있다면 그때는 따로 저장한다.
			pPass->SetVertexShader("CoreData/Shaders/ForwardLight.hlsl");
			//pPass->SetVertexShaderDefines("VERTEXCOLOR DIFFMAP");
			pPass->SetVertexShaderDefines("DIFFMAP");
			pPass->SetPixelShader("CoreData/Shaders/ForwardLight.hlsl");
			// 흐음 예상과 다르다. 순서를 맞춰야 한다.
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

		// xmfloat3와 xmvector 전부 speed와 delta를 곱할 수가 없다.
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
			pTransform->Rotate(0.0f, -2.0f, 0.0f);
		}
		if (pInput->KeyPress(DIK_RIGHT))
		{
			pTransform->Rotate(0.0f, 2.0f, 0.0f);
		}
		if (pInput->KeyPress(DIK_UP))
		{
			pTransform->Rotate(-2.0f, 0.0f, 0.0f);
		}
		if (pInput->KeyPress(DIK_DOWN))
		{
			pTransform->Rotate(2.0f, 0.0f, 0.0f);
		}
	}
}