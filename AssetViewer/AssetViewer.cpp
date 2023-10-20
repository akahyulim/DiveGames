#include "AssetViewer.h"

DEFINE_APPLICATION_MAIN(AssetViewer)

static std::unique_ptr<Dive::Texture2D> s_pDebugTex;

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

static void DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGui::PushID(label.c_str());

	ImGui::Columns(2);

	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::DragFloat("##X", &value, 0.1f, 0.0f, 0.0f, "%.2f");
	
	ImGui::Columns(1);

	ImGui::PopID();
}

static void DrawVec2Control(const std::string& label, DirectX::XMFLOAT2& values, float resetValue = 0.0f, float columnWidth = 100.0f, const std::string& a = "X", const std::string& b = "Y")
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
	if (ImGui::Button(a.c_str(), buttonSize))
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
	if (ImGui::Button(b.c_str(), buttonSize))
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
	m_CameraMoveSpeed(10.0f),
	m_CameraRotationSpeed(100.0f)
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

	// Subscribe Events
	SUBSCRIBE_EVENT(Dive::eEventType::Update, EVENT_HANDLER_PARAM(OnUpdate));
	SUBSCRIBE_EVENT(Dive::eEventType::PostUpdate, EVENT_HANDLER_PARAM(OnPostUpdate));
	SUBSCRIBE_EVENT(Dive::eEventType::WindowEvent, EVENT_HANDLER_PARAM(OnWindowEvent));
}

void AssetViewer::Start()
{
	initializeImGui();

	m_pCamera = Dive::Scene::CreateGameObject("MainCamera");
	Dive::Camera* pCamera = m_pCamera->AddComponent<Dive::Camera>();
	pCamera->SetViewSize(static_cast<float>(Dive::Graphics::GetWindowWidth()), static_cast<float>(Dive::Graphics::GetWindowHeight()));
	pCamera->SetBackgroundColor(DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	m_pCamera->GetComponent<Dive::Transform>()->SetPosition(0.0f, 0.0f, -10.0f);

	s_pDebugTex = std::make_unique<Dive::Texture2D>();
	s_pDebugTex->LoadFromFile("Assets/Textures/DokeV.jpeg");
}

void AssetViewer::Stop()
{
	// destroy ImGui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

void AssetViewer::OnPostUpdate(const Dive::Event& evnt)
{
	// dock window 설정

	// 현재 그냥 한 곳에 몰아서 구현했다.
	// 이제 슬슬 위젯으로 빼내자. 현재 이상한 부분이 많다.
	{
		// 이 부분만 떼어서 Begin으로 구분하기도 한다.
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}

		static bool bOpen = false;
		bool m_editor_begun = false;

		// main window
		{
			const auto window_flags =
				ImGuiWindowFlags_MenuBar |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus;

			// Set window position and size
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y));
			ImGui::SetNextWindowViewport(viewport->ID);

			// Set window style
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowBgAlpha(0.0f);

			// Begin window
			std::string name = "##main_window";
			bool open = true;
			m_editor_begun = ImGui::Begin(name.c_str(), &open, window_flags);
			ImGui::PopStyleVar(3);

			// Begin dock space
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable && m_editor_begun)
			{
				// Dock space
				const auto window_id = ImGui::GetID(name.c_str());
				if (!ImGui::DockBuilderGetNode(window_id))
				{
					// Reset current docking state
					ImGui::DockBuilderRemoveNode(window_id);
					ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
					ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

					// DockBuilderSplitNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_dir, ImGuiID* out_id_other);
					ImGuiID dock_main_id = window_id;
					ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
					ImGuiID dock_right_down_id = ImGui::DockBuilderSplitNode(dock_right_id, ImGuiDir_Down, 0.6f, nullptr, &dock_right_id);
					ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, nullptr, &dock_main_id);
					ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

					// Dock windows
					ImGui::DockBuilderDockWindow("World", dock_right_id);
					ImGui::DockBuilderDockWindow("Properties", dock_right_down_id);
					ImGui::DockBuilderDockWindow("Console", dock_down_id);
					ImGui::DockBuilderDockWindow("Assets", dock_down_right_id);
					ImGui::DockBuilderDockWindow("Viewport", dock_main_id);

					ImGui::DockBuilderFinish(dock_main_id);
				}

				ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
				ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
				ImGui::PopStyleVar();
			}
		}

		float viewportWidth, viewportHeight;
		// Viewport
		{
			ImGui::Begin("Viewport");

			// 크기가 영 이상하다.
			// 그리고 이 크기에 맞춰 Renderer::SetResolutionRender()를 해줘야 한다.
			auto width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
			auto height = ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y;

			viewportWidth = width;
			viewportHeight = height;

			auto pTexture = Dive::Renderer::GetRenderTarget(Dive::eRenderTarget::FrameRender);

			ImGui::Image(
				pTexture ? pTexture->GetShaderResourceView() : s_pDebugTex->GetShaderResourceView(),
				ImVec2(width, height),
				ImVec2(0, 0),
				ImVec2(1, 1),
				ImColor(255, 255, 255, 255),
				ImColor(50, 127, 166, 255)
			);

			ImGui::End();
		}

		// main menu
		{
			ImGui::Begin("Menu");

			if (ImGui::Button("Import"))
			{
				const std::string filePath = FileOpen("Assets/Models", "외부 모델 파일\0*.obj;*.dae;*.fbx;*.gltf\0");
				if (!filePath.empty())
				{
					if (m_AssetImporter.Load(filePath))
					{
						m_pLoadedModel = m_AssetImporter.GetModel()->GetRootGameObject();
						m_pSelectedNode = nullptr;
						m_MeshRenderers.clear();
						bOpen = true;
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Clear"))
			{
				m_AssetImporter.Clear();
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
			auto pTexture = Dive::Renderer::GetRenderTarget(Dive::eRenderTarget::FrameRender);
			ImGui::Text("Window Size: %d x %d", viewportWidth, viewportHeight);//pTexture ? pTexture->GetWidth() : Dive::Graphics::GetWindowWidth(), pTexture ? pTexture->GetHeight() : Dive::Graphics::GetWindowHeight());

			ImGui::End();
		}

		// camera
		{
			ImGui::Begin("Camera");

			// lookAt & position & rotation
			Dive::Transform* pCameraTransform = m_pCamera->GetTransform();
			DirectX::XMFLOAT3 lookAt;
			DirectX::XMStoreFloat3(&lookAt, pCameraTransform->GetLookAt());
			DrawVec3Control("LookAt", lookAt, 0.0f, 115.0f);
			pCameraTransform->LookAt(lookAt);
			DirectX::XMFLOAT3 position;
			DirectX::XMStoreFloat3(&position, pCameraTransform->GetLocalPosition());
			DrawVec3Control("Position", position, 0.0f, 115.0f);
			pCameraTransform->SetLocalPosition(position);
			DirectX::XMFLOAT3 rotation = pCameraTransform->GetLocalRotationDegrees();
			DrawVec3Control("Rotation", rotation, 0.0f, 115.0f);
			pCameraTransform->SetRotation(rotation);
			ImGui::Separator();


			// Clipping Planes
			Dive::Camera* pCamera = m_pCamera->GetComponent<Dive::Camera>();
			DirectX::XMFLOAT2 clippingPlanes;
			clippingPlanes.x = pCamera->GetNearClipPlane();
			clippingPlanes.y = pCamera->GetFarClipPlane();
			DrawVec2Control("Clipping Planes", clippingPlanes, 0.0f, 115.0f, "N", "F");
			pCamera->SetNearClipPlane(clippingPlanes.x);
			pCamera->SetFarClipPlane(clippingPlanes.y);
			ImGui::Separator();

			// Viewport Rect
			DirectX::XMFLOAT2 topLeft, viewSize;
			pCamera->GetViewportRectRate(topLeft.x, topLeft.y, viewSize.x, viewSize.y);
			DrawVec2Control("Viewport Rect", topLeft, 0.0f, 115.0f);
			DrawVec2Control("", viewSize, 1.0f, 115.0f, "W", "H");
			pCamera->SetViewportRectRate(topLeft.x, topLeft.y, viewSize.x, viewSize.y);
			ImGui::Separator();

			// Field of View
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 115.0f);
			ImGui::Text("Field of View");
			ImGui::NextColumn();
			int fov = static_cast<int>(pCamera->GetFieldOfView());
			ImGui::SliderInt("##Field of View", &fov, 1, 160);
			pCamera->SetFieldOfView(static_cast<float>(fov));
			ImGui::Columns(1);
			ImGui::Separator();

			// Control Speed
			DrawFloatControl("Move Speed", m_CameraMoveSpeed, 0.0f, 115.0f);
			DrawFloatControl("Rotation Speed", m_CameraRotationSpeed, 0.0f, 115.0f);
			ImGui::Separator();

			// Background
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 115.0f);
			ImGui::Text("Background");
			ImGui::NextColumn();
			ImGui::ColorEdit3("##Background", (float*)&m_ClearColor);
			pCamera->SetBackgroundColor(DirectX::XMFLOAT4(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w));
			ImGui::Columns(1);

			ImGui::End();
		}

		// Model Info
		if (bOpen)
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

					// BoneRenderer
					Dive::BoneRenderer* pBoneRenderer = m_pSelectedNode->GetComponent<Dive::BoneRenderer>();
					if (pBoneRenderer)
					{
						ImGui::Separator();

						if (ImGui::CollapsingHeader("Bone Renderer", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
						{
							// 이게 안된다.
							auto pBone = pBoneRenderer->GetBone();
							//if(pBone)
							//	ImGui::Text(pBone->name.c_str());
						}
					}

					// material
					Dive::IMeshRenderer* pMeshRenderer = m_pSelectedNode->GetComponent<Dive::IMeshRenderer>();
					if (pMeshRenderer)
					{
						ImGui::Separator();

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
								if (!filePath.empty())
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

		if (m_editor_begun)
		{
			ImGui::End();
		}
	}

	// imGui render
	{
		auto* pDeviceContext = Dive::Graphics::GetDeviceContext();
		auto* pDefaultRenderTargetView = Dive::Graphics::GetDefaultRenderTargetView();

		// ImGui를 default rendertargetview에 그린다.
		{
			auto* pRTV = Dive::Graphics::GetDefaultRenderTargetView();
			auto* pDSV = Dive::Graphics::GetDefaultDepthStencilView();

			Dive::Graphics::SetRenderTargetView(0, pRTV);
			Dive::Graphics::SetDepthStencilView(pDSV);

			Dive::Graphics::ClearViews(Dive::eClearTarget::Color | Dive::eClearTarget::Depth, DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0);
			pDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
		}

		// 이 부분을 떼어서 End로 구분하기도 한다.
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)Dive::Graphics::GetWindowWidth(), (float)Dive::Graphics::GetWindowHeight());

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}
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
