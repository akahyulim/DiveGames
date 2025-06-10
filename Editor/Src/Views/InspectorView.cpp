#include "InspectorView.h"
#include "imgui_internal.h"
#include "../Editor.h"

namespace Dive
{
	static void DrawVec3Control(const std::string& label, DirectX::XMFLOAT3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

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
		ImGui::PushFont(boldFont);
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
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, entt::entity entity, UIFunction uiFunction)
	{
		auto& entityManager = Engine::GetWorld()->GetEntityManager();

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entityManager.HasComponent<T>(entity))
		{
			auto& component = entityManager.GetComponent<T>(entity);
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entityManager.RemoveComponent<T>(entity);
		}
	}

	InspectorView::InspectorView(Editor* editor)
		: View(editor)
	{
		m_Title = "Inspector";
	}

	InspectorView::~InspectorView()
	{
	}
	
	void InspectorView::drawView()
	{
		auto& editorContext = EditorContext::GetInstance();
		if (!editorContext.ActiveWorld || editorContext.Selected == entt::null)
			return;

		auto& entityManager = editorContext.ActiveWorld->GetEntityManager();

		if (entityManager.HasComponent<NameComponent>(editorContext.Selected))
		{
			auto& name = entityManager.GetComponent<NameComponent>(editorContext.Selected).Name;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), name.c_str(), sizeof(buffer));
			if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}

		// 작동은 하지만 원하는 ui가 아니다.
		if (entityManager.HasComponent<TagComponent>(editorContext.Selected))
		{
			auto& tagCom = entityManager.GetComponent<TagComponent>(editorContext.Selected);

			if (ImGui::BeginMenu("Tag"))
			{
				if (ImGui::MenuItem("Untagged", nullptr, tagCom.Tag == TagComponent::eTag::Untagged))
					tagCom.Tag = TagComponent::eTag::Untagged;
				if (ImGui::MenuItem("Editor Only", nullptr, tagCom.Tag == TagComponent::eTag::EditorOnly))
					tagCom.Tag = TagComponent::eTag::EditorOnly;
				if (ImGui::MenuItem("Main Camera", nullptr, tagCom.Tag == TagComponent::eTag::MainCamera))
					tagCom.Tag = TagComponent::eTag::MainCamera;

				ImGui::EndMenu();
			}
		}

		if (entityManager.HasComponent<LocalTransform>(editorContext.Selected))
		{
			ImGui::Separator();

			auto& localToWorld = entityManager.GetComponent<LocalToWorld>(editorContext.Selected);

			auto position = Math::GetPositionFromTransform(localToWorld.Value);
			DrawVec3Control("Position", position);

			auto rotation = Math::QuaternionToDegrees(Math::GetRotationFromTransform(localToWorld.Value));
			DrawVec3Control("Rotation", rotation);

			auto scale = Math::GetScaleFromTransform(localToWorld.Value);
			DrawVec3Control("Scale", scale, 1.0f);

			if (entityManager.HasComponent<ParentComponent>(editorContext.Selected))
			{
				auto parent = entityManager.GetComponent<ParentComponent>(editorContext.Selected).Parent;
				auto& parentLocalToWorld = entityManager.GetComponent<LocalToWorld>(parent);

				auto parentPosition = Math::GetPositionFromTransform(parentLocalToWorld.Value);
				DirectX::XMMATRIX inverseTranslation = DirectX::XMMatrixTranslation(-parentPosition.x, -parentPosition.y, -parentPosition.z);
				auto localPosition = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&position), inverseTranslation);
				DirectX::XMStoreFloat3(&position, localPosition);

				auto parentRotation = Math::GetRotationFromTransform(parentLocalToWorld.Value);
				DirectX::XMVECTOR inverseRotation = DirectX::XMQuaternionInverse(DirectX::XMLoadFloat4(&parentRotation));
				DirectX::XMFLOAT4 localRotationQuaternion = Math::DegreesToQuaternion(rotation);
				DirectX::XMVECTOR localRotation = DirectX::XMQuaternionMultiply(DirectX::XMLoadFloat4(&localRotationQuaternion), inverseRotation);
				DirectX::XMStoreFloat4(&localRotationQuaternion, localRotation);
				rotation = Math::QuaternionToDegrees(localRotationQuaternion);

				auto parentScale = Math::GetScaleFromTransform(parentLocalToWorld.Value);
				DirectX::XMMATRIX inverseScale = DirectX::XMMatrixScaling(1.0f / parentScale.x, 1.0f / parentScale.y, 1.0f / parentScale.z);
				auto localScale = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&scale), inverseScale);
				DirectX::XMStoreFloat3(&scale, localScale);
			}

			auto& localTransform = entityManager.GetComponent<LocalTransform>(editorContext.Selected);
			localTransform.Position = position;
			localTransform.Rotation = Math::DegreesToQuaternion(rotation);
			localTransform.Scale = scale;
		}

		if (entityManager.HasComponent<CameraComponent>(editorContext.Selected))
		{
			ImGui::Separator();

			auto& cameraCom = entityManager.GetComponent<CameraComponent>(editorContext.Selected);
			ImGui::Text("Camera Type: %s", cameraCom.Type == CameraComponent::eProjectionType::Perspective ? "Perspective" : "Orthographic");
			ImGui::ColorEdit4("Clear Color", &cameraCom.ClearColor.x);
		}

		ImGui::Separator();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			displayAddComponentEntry<CameraComponent>("Camera");

			ImGui::EndPopup();
		}
	}	
}
