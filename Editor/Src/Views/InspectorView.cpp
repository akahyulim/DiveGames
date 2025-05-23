#include "InspectorView.h"
#include "imgui_internal.h"

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
		if (m_SelectedNode == entt::null)
			return;

		drawComponents(m_SelectedNode);
	}

	void InspectorView::drawComponents(entt::entity entity)
	{	
		auto& entityManager = Engine::GetWorld()->GetEntityManager();

		if (entityManager.HasComponent<NameComponent>(entity))
		{
			auto& name = entityManager.GetComponent<NameComponent>(entity).Name;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), name.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				name = std::string(buffer);
			}
		}

		if (entityManager.HasComponent<LocalTransform>(entity))
		{
			auto& localToWorld = entityManager.GetComponent<LocalToWorld>(entity);

			auto position = Math::GetPositionFromTransform(localToWorld.Value);
			DrawVec3Control("Position", position);

			auto rotation = Math::QuaternionToDegrees(Math::GetRotationFromTransform(localToWorld.Value)); 
			DrawVec3Control("Rotation", rotation);

			auto scale = Math::GetScaleFromTransform(localToWorld.Value);
			DrawVec3Control("Scale", scale, 1.0f);	

			if (entityManager.HasComponent<ParentComponent>(entity))
			{
				auto parent = entityManager.GetComponent<ParentComponent>(entity).Parent;
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

			auto& localTransform = entityManager.GetComponent<LocalTransform>(entity);
			localTransform.Position = position;
			localTransform.Rotation = Math::DegreesToQuaternion(rotation);
			localTransform.Scale = scale;
		}

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			displayAddComponentEntry<CameraComponent>("Camera");

			ImGui::EndPopup();
		}
	}
}
