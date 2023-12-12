#include "MenuTabsPanel.h"

#include <imgui-docking/Imgui.h>
#include <Imgui-docking/imgui_internal.h>
#include <imgui-docking/imgui_stdlib.h>

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
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

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

DirectX::XMFLOAT4 ImVec4ToXMFloat4(const ImVec4& value)
{
	return { value.x, value.y, value.z, value.w };
}

ImVec4 XMFloat4ToImVec4(const DirectX::XMFLOAT4& value)
{
	return { value.x, value.y, value.z, value.w };
}

MenuTabsPanel::MenuTabsPanel()
	: Panel("MenuTabs")
	, m_pLoadedModel(nullptr)
	, m_pCameraObject(nullptr)
{
}

MenuTabsPanel::~MenuTabsPanel()
{
}

void MenuTabsPanel::Initialize(Dive::GameObject* pCameraObject)
{
	DV_ASSERT(pCameraObject);

	m_pCameraObject = pCameraObject;
}

// CameraBackgroundColor, Skydome ���� �� Ȱ���� ���ο� ���� ����ȴ�.
// �̴� ����� ���ڿ�������.
void MenuTabsPanel::OnRender()
{
	ImGui::Begin(GetPanelName().c_str(), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
	
	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("File"))
		{
			showFileTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera"))
		{
			showCameraTab();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Environment"))
		{
			showEnvironmentTab();
			ImGui::EndTabItem();
		}

		if (m_pLoadedModel)
		{
			if (ImGui::BeginTabItem("Model"))
			{
				showModelTab();
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End();	// MenuTabs
}

void MenuTabsPanel::showFileTab()
{
	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::NewLine();

	if (ImGui::Button("Open"))
	{
		const std::string filePath = FileOpen("Assets/Models", "�ܺ� �� ����\0*.obj;*.dae;*.fbx;*.gltf\0");
		if (!filePath.empty())
		{
			// ������ �߸��ƴ�.
			// �����ͷκ��� �ε��ϴ� ���� �ƴ϶�
			// �𵨰�ü�� �ε� �Լ��� ��θ� �����ϸ� ���ο��� �����ͷ� �� ���� ������Ʈ�� ������ �� ������� �ؾ��Ѵ�.
			if (m_ModelImporter.Load(filePath))
			{
				m_pLoadedModel = m_ModelImporter.GetImportedModel()->GetRootGameObject();
			}
		}
	}

	ImGui::NewLine();
	ImGui::NewLine();

	if (ImGui::Button("Save Model"))
	{
	}

	ImGui::NewLine();
	ImGui::NewLine();

	if (ImGui::Button("Save Scene"))
	{
	}

	ImGui::NewLine();
	ImGui::NewLine();

	if (ImGui::Button("Exit"))
	{
		Dive::Engine::Exit();
	}
}

void MenuTabsPanel::showCameraTab()
{
	DV_ASSERT(m_pCameraObject);

	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::NewLine();

	Dive::Transform* pTransformCom = m_pCameraObject->GetTransform();
	Dive::Camera* pCameraCom = m_pCameraObject->GetComponent<Dive::Camera>();

	// lookAt & position & rotation
	//DirectX::XMFLOAT3 lookAt = pTransformCom->GetLookAt();
	//DrawVec3Control("LookAt", lookAt, 0.0f, 115.0f);
	//pTransformCom->SetLookAt(lookAt);

	DirectX::XMFLOAT3 position = pTransformCom->GetLocalPosition();
	DrawVec3Control("Position", position, 0.0f, 115.0f);
	pTransformCom->SetLocalPosition(position);
	
	DirectX::XMFLOAT3 rotation = pTransformCom->GetLocalRotationDegree();
	DrawVec3Control("Rotation", rotation, 0.0f, 115.0f);
	pTransformCom->SetRotation(rotation);
	
	ImGui::Separator();

	// Clipping Planes
	static DirectX::XMFLOAT2 clippingPlanes = { 0.0f, 0.0f };
	
	clippingPlanes.x = pCameraCom->GetNearClipPlane();
	clippingPlanes.y = pCameraCom->GetFarClipPlane();
	DrawVec2Control("Clipping Planes", clippingPlanes, 0.0f, 115.0f, "N", "F");
	pCameraCom->SetNearClipPlane(clippingPlanes.x);
	pCameraCom->SetFarClipPlane(clippingPlanes.y);
	
	ImGui::Separator();

	// Viewport Rect
	static DirectX::XMFLOAT2 topLeft = { 0.0f, 0.0f };
	static DirectX::XMFLOAT2 viewSize = { 0.0f, 0.0f };
	
	pCameraCom->GetViewportRectRate(topLeft.x, topLeft.y, viewSize.x, viewSize.y);
	DrawVec2Control("Viewport Rect", topLeft, 0.0f, 115.0f);
	DrawVec2Control("", viewSize, 1.0f, 115.0f, "W", "H");
	pCameraCom->SetViewportRectRate(topLeft.x, topLeft.y, viewSize.x, viewSize.y);
	
	ImGui::Separator();

	// Field of View
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 115.0f);
	ImGui::Text("Field of View");
	ImGui::NextColumn();
	static int fov = static_cast<int>(pCameraCom->GetFieldOfView());
	ImGui::SliderInt("##Field of View", &fov, 1, 160);
	pCameraCom->SetFieldOfView(static_cast<float>(fov));
	ImGui::Columns(1);
	
	ImGui::Separator();

	// Control Speed
	static float camMoveSpeed = pCameraCom->GetMoveSpeed();
	static float camRotateSpeed = pCameraCom->GetRotateSpeed();
	
	DrawFloatControl("Move Speed", camMoveSpeed, 0.0f, 115.0f);
	DrawFloatControl("Rotate Speed", camRotateSpeed, 0.0f, 115.0f);
	pCameraCom->SetMoveSpeed(camMoveSpeed);
	pCameraCom->SetRotateSpeed(camRotateSpeed);
	
	ImGui::Separator();

	// Background
	static ImVec4 clearColor = XMFloat4ToImVec4(pCameraCom->GetBackgroundColor());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 115.0f);
	ImGui::Text("Background");
	ImGui::NextColumn();
	ImGui::ColorEdit3("##Background", (float*)&clearColor);
	pCameraCom->SetBackgroundColor(ImVec4ToXMFloat4(clearColor));
	ImGui::Columns(1);
}

void MenuTabsPanel::showEnvironmentTab()
{
	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::NewLine();

	// skydome
	{
		auto pSkydome = m_pCameraObject->GetComponent<Dive::Camera>()->GetSkydome();

		static ImVec4 apexColor = XMFloat4ToImVec4(pSkydome->GetApexColor());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("ApexColor");
		ImGui::NextColumn();
		ImGui::ColorEdit3("##ApexColor", (float*)&apexColor);
		pSkydome->SetApexColor(ImVec4ToXMFloat4(apexColor));
		ImGui::Columns(1);

		static ImVec4 centerColor = XMFloat4ToImVec4(pSkydome->GetCenterColor());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("CenterColor");
		ImGui::NextColumn();
		ImGui::ColorEdit3("##CenterColor", (float*)&centerColor);
		pSkydome->SetCenterColor(ImVec4ToXMFloat4(centerColor));
		ImGui::Columns(1);
	}

	ImGui::Separator();

	// light
	{
		// directional light
		static ImVec4 dirLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("Directional Light");
		ImGui::NextColumn();
		ImGui::ColorEdit3("##DirLightColor", (float*)&dirLightColor);
		//pCameraCom->SetBackgroundColor(DirectX::XMFLOAT4(s_ClearColor.x, s_ClearColor.y, s_ClearColor.z, s_ClearColor.w));
		ImGui::Columns(1);

		// X�� ȸ��
		static int dirLightRotX = 0;
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("Rotation X");
		ImGui::NextColumn();
		ImGui::SliderInt("##DirLightRotX", &dirLightRotX, 0, 359);
		//pCameraCom->SetFieldOfView(static_cast<float>(fov));
		ImGui::Columns(1);

		// Y�� ȸ��
		static int dirLightRotY = 0;
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("Rotation Y");
		ImGui::NextColumn();
		ImGui::SliderInt("##DirLightRotY", &dirLightRotY, 0, 359);
		//pCameraCom->SetFieldOfView(static_cast<float>(fov));
		ImGui::Columns(1);

		// Z�� ȸ��
		static int dirLightRotZ = 0;
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("Rotation Z");
		ImGui::NextColumn();
		ImGui::SliderInt("##DirLightRotZ", &dirLightRotZ, 0, 359);
		//pCameraCom->SetFieldOfView(static_cast<float>(fov));
		ImGui::Columns(1);

		ImGui::Separator();

		// point light
		static ImVec4 pointLightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("Point Light");
		ImGui::NextColumn();
		ImGui::ColorEdit3("##PointLightColor", (float*)&pointLightColor);
		//pCameraCom->SetBackgroundColor(DirectX::XMFLOAT4(s_ClearColor.x, s_ClearColor.y, s_ClearColor.z, s_ClearColor.w));
		ImGui::Columns(1);

		static int pointLightHeight = 10;
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 115.0f);
		ImGui::Text("Height");
		ImGui::NextColumn();
		ImGui::SliderInt("##PointLightHeight", &pointLightHeight, 10, 1000);
		//pCameraCom->SetFieldOfView(static_cast<float>(fov));
		ImGui::Columns(1);

		// �ݰ浵 �߰�
	}
}

void MenuTabsPanel::showModelTab()
{
	ImGui::NewLine();
	ImGui::NewLine();
	ImGui::NewLine();

	auto pTransformCom = m_pLoadedModel->GetTransform();

	static float scale = 1.0f;
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 115.0f);
	ImGui::Text("Scale");
	ImGui::NextColumn();
	ImGui::SliderFloat("##Scale", &scale, 0.01f, 10.0f, "%0.2f");
	pTransformCom->SetScale(scale, scale, scale);
	ImGui::Columns(1);

	// �̵� �� ȸ���� ImGuizmo Ȱ��?

	// �ִϸ��̼��� �����Ѵٸ�
	// �ڵ�����, ���� �ӵ� ����?
	// �ٸ� �ִϸ��̼� ����?
}
