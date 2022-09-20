#include "Sandbox.h"

DEFINE_APPLICATION_MAIN(Sandbox::Sandbox)

namespace Sandbox
{
	Sandbox::Sandbox(Dive::Context* pContext)
		: Dive::Application(pContext),
		m_pScene(nullptr),
		m_pTriangleModel(nullptr)
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
			testSaveModel();
			testCreateTriangleModel();
			testResourceCache();
			testFileSystem();

			auto* pTriangle = m_pScene->CreateGameObject("Triangle");
			auto* pStaticModel = pTriangle->CreateComponent<Dive::StaticModel>();
			pStaticModel->SetModel(m_pTriangleModel);
		}

		// camera�� ��� ������ ���� ����

		// create ui content

		// setup viewport
		// 1. viewport�� ������ �� renderer�� set
		// viewport�� scene, camera �׸��� �ʿ��ϴٸ� render path�� �����Ͽ� ����
		// 2. render path�� ���ٸ� cache�κ��� load�� �� viewport�� set
		auto pView = std::make_shared<Dive::View>(m_pContext, m_pScene);
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

	void Sandbox::testSaveModel()
	{
		// ���� ���� ����(ShadowData�� ���� ����.)
		std::vector<Dive::VertexElement> elements;
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_POSITION);
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_NORMAL);
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_COLOR);

		auto pVb = new Dive::VertexBuffer(m_pContext);
		pVb->SetSize(3, elements);

		// �ε��� ���� ����
		unsigned short indices[] = { 0, 1, 2 };
		auto pIb = new Dive::IndexBuffer(m_pContext);
		pIb->SetSize(3, false);
		{
			auto* pDest = (unsigned short*)pIb->GetShadowData();
			*pDest++ = indices[0];	*pDest++ = indices[1];	*pDest++ = indices[2];
		}

		// �޽� ����
		auto pMesh = new Dive::Mesh(m_pContext);
		pMesh->SetVertexBuffer(0, pVb);
		pMesh->SetIndexBuffer(pIb);

		// �� ����
		auto pModel = new Dive::Model(m_pContext);
		std::vector<Dive::VertexBuffer*> vertexBuffers;
		vertexBuffers.emplace_back(pVb);
		std::vector<Dive::IndexBuffer*> indexBuffers;
		indexBuffers.emplace_back(pIb);
		pModel->SetNumMeshes(1);
		pModel->SetMesh(0, pMesh);
		pModel->SetVertexBuffers(vertexBuffers);
		pModel->SetIndexBuffers(indexBuffers);

		// �� ���̺�
		pModel->Save(nullptr);
	}

	void Sandbox::testCreateTriangleModel()
	{
		// ���� ���� ����
		std::vector<Dive::VertexElement> elements;
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_POSITION);
		elements.emplace_back(Dive::TYPE_VECTOR3, Dive::SEM_COLOR);

		float vertices[] = { 
			-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f,					// pos
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f	// color
		};

		auto* pVb = new Dive::VertexBuffer(m_pContext);
		pVb->CreateBuffer(3, elements, (const void*)vertices);

		// �ε��� ���� ����
		unsigned short indices[] = { 0, 1, 2 };
		
		auto* pIb = new Dive::IndexBuffer(m_pContext);
		pIb->CreateBuffer(3, false, (const void*)indices);

		// �޽� ����
		auto* pMesh = new Dive::Mesh(m_pContext);
		pMesh->SetNumVertexBuffers(1);
		pMesh->SetVertexBuffer(0, pVb);
		pMesh->SetIndexBuffer(pIb);
		pMesh->SetPrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// �� ����
		std::vector<Dive::VertexBuffer*> vertexBuffers;
		vertexBuffers.emplace_back(pVb);
		std::vector<Dive::IndexBuffer*> indexBuffers;
		indexBuffers.emplace_back(pIb);

		m_pTriangleModel = new Dive::Model(m_pContext);
		m_pTriangleModel->SetNumMeshes(1);
		m_pTriangleModel->SetMesh(0, pMesh);
		m_pTriangleModel->SetVertexBuffers(vertexBuffers);
		m_pTriangleModel->SetIndexBuffers(indexBuffers);
	}

	void Sandbox::testResourceCache()
	{
		auto* pModelA = new Dive::Model(m_pContext);
		pModelA->SetName("ModelA.md");
		auto* pModelB = new Dive::Model(m_pContext);
		pModelB->SetName("ModelB.md");
		auto* pModelC = new Dive::Model(m_pContext);
		pModelC->SetName("ModelC.md");
		auto* pModelD = new Dive::Model(m_pContext);
		pModelD->SetName("ModelD.md");
		auto* pModelE = new Dive::Model(m_pContext);
		pModelE->SetName("ModelE.md");

		auto* pResourceCache = GetSubsystem<Dive::ResourceCache>();
		pResourceCache->AddResource(pModelA);
		pResourceCache->AddResource(pModelB);
		pResourceCache->AddResource(pModelC);
		pResourceCache->AddResource(pModelD);
		pResourceCache->AddResource(pModelE);

		// get
		auto* pModel = pResourceCache->GetResource<Dive::Model>("ModelA.md");
		DV_LOG_CLIENT_DEBUG("Current Resource Name: {:s}", pModel->GetName());
		pModel = pResourceCache->GetResource<Dive::Model>("ModelC.md");
		DV_LOG_CLIENT_DEBUG("Current Resource Name: {:s}", pModel->GetName());
		pModel = pResourceCache->GetResource<Dive::Model>("ModelD.md");
		DV_LOG_CLIENT_DEBUG("Current Resource Name: {:s}", pModel->GetName());

		std::vector<Dive::Model*> models;
		pResourceCache->GetResources<Dive::Model>(models);
		for (auto* pCurModel : models)
		{
			DV_LOG_CLIENT_DEBUG("SameType Resource Name: {:s}", pCurModel->GetName());
		}

		// release
		pResourceCache->RemoveResource<Dive::Model>("ModelB.md");
		pResourceCache->RemoveResource<Dive::Model>("ModelC.md");
		pResourceCache->GetResources<Dive::Model>( models);
		for (auto* pCurModel : models)
		{
			DV_LOG_CLIENT_DEBUG("Existed Resource Name: {:s}", pCurModel->GetName());
		}

		pResourceCache->RemoveResources<Dive::Model>();
		pResourceCache->GetResources<Dive::Model>(models);
		if (models.empty())
		{
			DV_LOG_CLIENT_DEBUG("removed all Resources");
		}
	}

	void Sandbox::testFileSystem()
	{
		auto* pFileSystem = GetSubsystem<Dive::FileSystem>();

		DV_LOG_CLIENT_DEBUG("ToLowercase: {:s}", pFileSystem->ToLowerCase("AaBbCcDdEe"));
		DV_LOG_CLIENT_DEBUG("ToUppercase: {:s}", pFileSystem->ToUpperCase("AaBbCcDdEe"));

		std::string fullPath = "C:\\Dev\\Projects\\DiveGames\\CoreData\\Shaders\\test.hlsl";
		DV_LOG_CLIENT_DEBUG("InternalPath: {:s}", pFileSystem->GetInternalPath(fullPath));
		DV_LOG_CLIENT_DEBUG("NativePath: {:s}", pFileSystem->GetNativePath(pFileSystem->GetInternalPath(fullPath)));

		std::string path, name, extension;
		pFileSystem->SplitPath(fullPath, path, name, extension);
		DV_LOG_CLIENT_DEBUG("Path: {0:s}, FileName: {1:s}, Extension: {2:s}", path, name, extension);
		DV_LOG_CLIENT_DEBUG("Path: {0:s}, FileName: {1:s}, Extension: {2:s}", pFileSystem->GetPath(fullPath), pFileSystem->GetFileName(fullPath), pFileSystem->GetExtension(fullPath));

		DV_LOG_CLIENT_DEBUG("FileName+Extension: {:s}", pFileSystem->GetFileNameAndExtension(fullPath));

		DV_LOG_CLIENT_DEBUG("ParentPath: {:s}", pFileSystem->GetParentPath(fullPath));

		DV_LOG_CLIENT_DEBUG("Org: {0:s}, Trimmed: {1:s}", "   Knave ", pFileSystem->StringTrim("   Knave "));

		DV_LOG_CLIENT_DEBUG("AbsolutePath: {0:b}, RelativePath: {1:b}", pFileSystem->IsAbsolutePath(fullPath), pFileSystem->IsAbsolutePath("../CoreData/Shaders/test.hlsl"));

		// copy
		pFileSystem->Copy("C:/Users/akahy/Desktop/RenderPath.txt", "C:/Knave/Copied_RenderPath.txt");
		// rename
		pFileSystem->Rename("C:/Knave/Copied_RenderPath.txt", "C:/Knave/RenderPath.txt");
		// delete
		pFileSystem->Delete("C:/Knave/RenderPath.txt");

		DV_LOG_CLIENT_DEBUG("Current Path: {:s}", Dive::FileSystem::GetCurrentDir());
		Dive::FileSystem::SetCurrentDir("C:/");
		DV_LOG_CLIENT_DEBUG("Current Path: {:s}", Dive::FileSystem::GetCurrentDir());

		std::string dir = Dive::FileSystem::AddTrailingSlash("dev/awsome_game");
		DV_LOG_CLIENT_DEBUG("add: {:s}", dir);
		DV_LOG_CLIENT_DEBUG("remove: {:s}", Dive::FileSystem::RemoveTrailingSlash(dir));
	}
}