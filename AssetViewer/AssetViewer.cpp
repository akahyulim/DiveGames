#include "AssetViewer.h"

DEFINE_APPLICATION_MAIN(AssetViewer)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static std::string FileOpen(const char* pDir, const char* pFilter = nullptr)
{
	char filePath[MAX_PATH] = { 0 };
	std::string initialDir = Dive::FileSystem::GetNativePath(Dive::FileSystem::GetCurrentDir() + pDir);

	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = Dive::Graphics::GetWindowHandle();
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = sizeof(filePath);
	ofn.lpstrInitialDir = initialDir.c_str();
	ofn.lpstrFilter = (pFilter != nullptr) ? pFilter : "All Files(*.*)\0";
	ofn.nFilterIndex = 1;
	ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (!GetOpenFileNameA(&ofn))
		return std::string();

	return ofn.lpstrFile;
}
static void DrawVec2Control(const std::string& label, DirectX::XMFLOAT2& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto pBoldFont = io.Fonts->Fonts[1];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(pBoldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(pBoldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	
	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

static void DrawVec3Control(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto pBoldFont = io.Fonts->Fonts[1];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(pBoldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(pBoldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(pBoldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.3f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

AssetViewer::AssetViewer()
	: m_pCamera(nullptr),
	m_pLoadedModel(nullptr),
	m_pSelectedNode(nullptr),
	m_Fps(0),
	m_CameraMoveSpeed(5.0f),
	m_CameraRotationSpeed(10.0f)
{
	m_ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

AssetViewer::~AssetViewer()
{
}

void AssetViewer::Setup()
{
	// Engine Setup
	Dive::Graphics::SetWindowTitle(L"AssetViewer");
	//Dive::Graphics::ResizeWindow(800, 600);

	// Subscribe Events
	SUBSCRIBE_EVENT(Dive::eEventType::BeginRender, EVENT_HANDLER_PARAM(OnBeginRender));
	SUBSCRIBE_EVENT(Dive::eEventType::EndRender, EVENT_HANDLER_PARAM(OnEndRender));
	SUBSCRIBE_EVENT(Dive::eEventType::WindowEvent, EVENT_HANDLER_PARAM(OnWindowEvent));
	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
}

void AssetViewer::Start()
{
	initializeImGui();

	m_pCamera = Dive::Scene::CreateGameObject("MainCamera");
	Dive::Camera* pCamera = m_pCamera->AddComponent<Dive::Camera>();
	pCamera->SetAspectRatio(static_cast<float>(Dive::Graphics::GetWindowWidth()), static_cast<float>(Dive::Graphics::GetWindowHeight()));
	m_pCamera->GetComponent<Dive::Transform>()->SetPosition(0.0f, 0.0f, -10.0f);
}

void AssetViewer::Stop()
{
	// destroy ImGui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void AssetViewer::OnBeginRender(const Dive::Event& e)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	static bool bOpen = false;

	// main menu
	{
		ImGui::Begin("Menu");

		if (ImGui::Button("Import"))
		{
			const std::string filePath = FileOpen("Assets/Models", "외부 모델 파일\0*.obj;*.dae;*.fbx\0");
			if (!filePath.empty())
			{
				if (m_Importer.LoadFromFile(filePath))
				{
					m_pLoadedModel = m_Importer.GetModel()->GetRootGameObject();
					m_pSelectedNode = nullptr;
					m_MeshRenderers.clear();
					bOpen = true;
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear"))
		{
			m_Importer.Clear();
			m_pLoadedModel = nullptr;
			m_pSelectedNode = nullptr;
			m_MeshRenderers.clear();
			bOpen = false;
		}

		if (ImGui::Button("Export"))
		{
			// model
			// material
			// animation
		}

		if (ImGui::Button("Exit"))
		{
			Dive::Engine::Exit();
		}

		ImGui::End();
	}

	// info
	{
		ImGui::Begin("Info");

		// fps
		ImGui::Text("fps: %d", m_Fps);
		
		// window size
		ImGui::Text("Window Size: %d x %d", Dive::Graphics::GetWindowWidth(), Dive::Graphics::GetWindowHeight());

		ImGui::End();
	}

	// camera
	{
		ImGui::Begin("Camera");

		// position
		Dive::Transform* pCameraTransform = m_pCamera->GetTransform();
		DirectX::XMFLOAT3 position;
		DirectX::XMStoreFloat3(&position, pCameraTransform->GetLocalPosition());
		DrawVec3Control("Position", position, 0.0f, 115.0f);
		pCameraTransform->SetLocalPosition(position);

		// near & far plane
		Dive::Camera* pCamera = m_pCamera->GetComponent<Dive::Camera>();
		DirectX::XMFLOAT2 cameraPlane;
		cameraPlane.x = pCamera->GetNearClipPlane();
		cameraPlane.y = pCamera->GetFarClipPlane();
		DrawVec2Control("near & far Plane", cameraPlane, 0.0f, 115.0f);
		pCamera->SetNearClipPlane(cameraPlane.x);
		pCamera->SetFarClipPlane(cameraPlane.y);

		// camera pov
		//DirectX::XMFLOAT2 cameraPov;
		//cameraPov.x = pCamera->GetAspectRatio();
		//cameraPov.y = pCamera->GetNearClipPlane();
		//DrawVec2Control("Camera Pov", cameraPov, 0.0f, 120.0f);
		//pCamera->SetAspectRatio(cameraPov.x, cameraPov.y);

		// Move speed
		ImGui::Text("Move Speed");
		ImGui::SameLine();
		ImGui::SliderFloat("##MoveSpeed", &m_CameraMoveSpeed, 1.0f, 1000.0f, "%.1f");

		ImGui::Text("Rotation Speed");
		ImGui::SameLine();
		ImGui::SliderFloat("##RotationSpeed", &m_CameraRotationSpeed, 1.0f, 500.0f, "%.1f");

		// clear color
		ImGui::ColorEdit3("Clear Color", (float*)&m_ClearColor);

		ImGui::End();
	}

	// Model Info
	if(bOpen)
	{
		// Model Hierarchy
		{
			if (!ImGui::Begin("Hierarchy"))
			{
				ImGui::End();
				return;
			}
			
			drawTree(m_pLoadedModel);
			
			ImGui::End();
		}

		if (m_pSelectedNode)
		{
			ImGui::Begin("Inspector");
			{
				// transform
				Dive::Transform* pTransform = m_pSelectedNode->GetComponent<Dive::Transform>();
				if (pTransform)
				{
					if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
					{
						// position
						DirectX::XMFLOAT3 position;
						DirectX::XMStoreFloat3(&position, pTransform->GetLocalPosition());
						DrawVec3Control("Position", position);
						pTransform->SetLocalPosition(position);

						// rotation
						auto rotation = pTransform->GetLocalRotationDegrees();
						DrawVec3Control("Rotation", rotation);
						pTransform->SetLocalRotation(rotation);

						// scale
						DirectX::XMFLOAT3 scale;
						DirectX::XMStoreFloat3(&scale, pTransform->GetLocalScale());
						DrawVec3Control("Scale", scale, 1.0f);
						pTransform->SetLocalScale(scale);
					}
				}

				ImGui::Separator();

				// material
				Dive::IMeshRenderer* pMeshRenderer = m_pSelectedNode->GetComponent<Dive::IMeshRenderer>();
				if (pMeshRenderer)
				{
					if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
					{
						Dive::Material* pMat = pMeshRenderer->GetMaterial();

						// Material을 직접 만들 수 있어야 한다.
						// 이때 저장도 되어야 한다.
						// 그리고 로드도 가능해야 한다.
						ImGui::Text(pMat->GetName().c_str());

						ImGui::Text("Diffuse: %s", pMat->GetTexture(Dive::eTextureUnit::Diffuse)->GetName().c_str());

						ImGui::SameLine();

						if (ImGui::Button("Load"))
						{
							const std::string filePath = FileOpen("Assets/Models");
							if(!filePath.empty())
								pMat->AddTexture(Dive::eTextureUnit::Diffuse, filePath);
						}

						ImGui::Text("Normal");

						ImGui::Text("Specular");

						// titling
						DirectX::XMFLOAT2 tiling = pMat->GetTiling();
						DrawVec2Control("Tiling", tiling, 1.0f);
						pMat->SetTiling(tiling);
						
						// offset
						DirectX::XMFLOAT2 offset = pMat->GetOffset();
						DrawVec2Control("Offset", offset);
						pMat->SetOffset(offset);
					}
				}

				ImGui::End();
			}
		}
	}
}

void AssetViewer::OnEndRender(const Dive::Event& e)
{
	auto* pDeviceContext = Dive::Graphics::GetDeviceContext();
	auto* pDefaultRenderTargetView = Dive::Graphics::GetDefaultRenderTargetView();

	ImGui::Render();

	{
		DirectX::XMFLOAT4 clearColor = { m_ClearColor.x * m_ClearColor.w, m_ClearColor.y * m_ClearColor.w, m_ClearColor.z * m_ClearColor.w, m_ClearColor.w };

		auto* pRTV = Dive::Graphics::GetDefaultRenderTargetView();
		auto* pDSV = Dive::Graphics::GetDefaultDepthStencilView();

		Dive::Graphics::SetRenderTargetView(0, pRTV);
		Dive::Graphics::SetDepthStencilView(pDSV);

		Dive::Graphics::ClearViews(Dive::eClearTarget::Color | Dive::eClearTarget::Depth, clearColor, 1.0f, 0);

		pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
	}

	// View를 사용하지 않으므로 직접 render path를 구성한다.
	// 여기에서 그려야 UI 뒤쪽이 된다.
	{
		// Graphics의 ClearViews에서 prepareDraw()를 호출한다.
		// 즉, 현재 구현에서는 이 부분을 건너 띄었다.

		{
			//Dive::Graphics::GetDeviceContext()->RSSetState(Dive::Renderer::GetRasterizerState());

			m_pCamera->GetComponent<Dive::Camera>()->SetAspectRatio(
				static_cast<float>(Dive::Graphics::GetWindowWidth()), static_cast<float>(Dive::Graphics::GetWindowHeight()));

			D3D11_VIEWPORT viewport;
			viewport.Width = static_cast<float>(Dive::Graphics::GetWindowWidth());
			viewport.Height = static_cast<float>(Dive::Graphics::GetWindowHeight());
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			Dive::Graphics::GetDeviceContext()->RSSetViewports(1, &viewport);

			auto pBuffer = Dive::Renderer::GetCameraVertexShaderBuffer();
			auto pMappedData = static_cast<Dive::CameraVertexShaderBuffer*>(pBuffer->Map());
			pMappedData->viewMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetComponent<Dive::Camera>()->GetViewMatrix());
			pMappedData->projMatrix = DirectX::XMMatrixTranspose(m_pCamera->GetComponent<Dive::Camera>()->GetProjectionMatrix());
			pBuffer->Unmap();
			Dive::Graphics::SetConstantBuffer(0, Dive::eShaderType::VertexShader, pBuffer);
		}

		// mesh renderers
		{
			auto it = m_MeshRenderers.begin();
			for (it; it != m_MeshRenderers.end(); ++it)
			{
				(*it)->Draw();
			}
		}
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void AssetViewer::OnWindowEvent(const Dive::Event& e)
{
	auto& evnt = dynamic_cast<const Dive::WindowEvent&>(e);

	ImGui_ImplWin32_WndProcHandler(
		evnt.m_hWnd,
		evnt.m_Msg,
		evnt.m_wParam,
		evnt.m_lParam
	);
}

void AssetViewer::initializeImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplWin32_Init(Dive::Graphics::GetWindowHandle());
	ImGui_ImplDX11_Init(
		Dive::Graphics::GetDevice(),
		Dive::Graphics::GetDeviceContext()
	);

	// Set ImGui Theme
	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// load resources
	float fontSize = 15.0f;
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothic.ttf", fontSize);
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/NanumBarunGothicBold.ttf", fontSize);
}

void AssetViewer::OnUpdate(const Dive::Event& evnt)
{
	// View를 사용하지 않으므로 직접 구성한다.
	if (m_pLoadedModel)
	{
		m_MeshRenderers.clear();

		const auto& allGameObjects = Dive::Scene::GetAllGameObjects();
		for (auto pGameObject : allGameObjects)
		{
			if (pGameObject->HasComponent<Dive::MeshRenderer>())
				m_MeshRenderers.emplace_back(pGameObject->GetComponent<Dive::MeshRenderer>());
			else if (pGameObject->HasComponent<Dive::SkinnedMeshRenderer>())
				m_MeshRenderers.emplace_back(pGameObject->GetComponent<Dive::SkinnedMeshRenderer>());
		}
	}

	// calcu fps
	{
		static double lastTime = Dive::Timer::GetTimeMS();
		static uint32_t tickCount = 0;
		tickCount++;

		if (Dive::Timer::GetTimeMS() >= (lastTime + 1000.0))
		{
			m_Fps = tickCount;
			tickCount = 0;
			lastTime = Dive::Timer::GetTimeMS();
		}
	}

	if (!m_pCamera)
		return;

	const auto& e = dynamic_cast<const Dive::UpdateEvent&>(evnt);
	float deltaTime = static_cast<float>(Dive::Timer::GetDeltaTimeSec());

	auto pTransform = m_pCamera->GetTransform();
	if (Dive::Input::KeyPress(DIK_W))
	{
		pTransform->Translate(0.0f, 0.0f, m_CameraMoveSpeed * deltaTime);
	}
	if (Dive::Input::KeyPress(DIK_S))
	{
		pTransform->Translate(0.0f, 0.0f, -m_CameraMoveSpeed * deltaTime);
	}
	if (Dive::Input::KeyPress(DIK_A))
	{
		pTransform->Translate(-m_CameraMoveSpeed * deltaTime, 0.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_D))
	{
		pTransform->Translate(m_CameraMoveSpeed * deltaTime, 0.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_C))
	{
		pTransform->Translate(0.0f, -m_CameraMoveSpeed * deltaTime, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_SPACE))
	{
		pTransform->Translate(0.0f, m_CameraMoveSpeed * deltaTime, 0.0f);
	}

	if (Dive::Input::KeyPress(DIK_Q))
	{
		pTransform->Rotate(0.0f, 0.0f, -m_CameraRotationSpeed * deltaTime);
	}
	if (Dive::Input::KeyPress(DIK_E))
	{
		pTransform->Rotate(0.0f, 0.0f, m_CameraRotationSpeed * deltaTime);
	}
	if (Dive::Input::KeyPress(DIK_LEFT))
	{
		pTransform->Rotate(0.0f, -m_CameraRotationSpeed * deltaTime, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_RIGHT))
	{
		pTransform->Rotate(0.0f, m_CameraRotationSpeed * deltaTime, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_UP))
	{
		pTransform->Rotate(-m_CameraRotationSpeed * deltaTime, 0.0f, 0.0f);
	}
	if (Dive::Input::KeyPress(DIK_DOWN))
	{
		pTransform->Rotate(m_CameraRotationSpeed * deltaTime, 0.0f, 0.0f);
	}
}

void AssetViewer::drawTree(Dive::GameObject* pNode)
{
	if (!pNode)
		return;

	auto children = pNode->GetTransform()->GetChildren();

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;
	nodeFlags |= (children.empty()) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

	if (m_pSelectedNode)
	{
		nodeFlags |= (m_pSelectedNode == pNode) ? ImGuiTreeNodeFlags_Selected : 0;
	}

	bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)pNode->GetID(), nodeFlags, pNode->GetName().c_str());
	
	if (ImGui::IsItemClicked())
	{
		m_pSelectedNode = pNode;
	}

	if (nodeOpen)
	{
		for (Dive::Transform* pChild : children)
			drawTree(pChild->GetGameObject());

		ImGui::TreePop();
	}
}
