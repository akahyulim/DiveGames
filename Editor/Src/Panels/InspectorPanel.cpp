#include "InspectorPanel.h"
#include "HierarchyPanel.h"

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
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

InspectorPanel::InspectorPanel(Editor* pEditor)
	: Panel(pEditor, "Inspector")
{
	// �ּ� ũ�� Ȯ���� �ʿ��ϴ�.
	// ��� ũ�⸦ �����ϴ� ���� �������ϴµ� �� ���ϴ�.

	SUBSCRIBE_EVENT(eEventType::CreateScene, EVENT_HANDLER(OnActiveScene));
}

InspectorPanel::~InspectorPanel()
{
}

void InspectorPanel::OnActiveScene(const Dive::Event& e)
{
	const auto& evt = dynamic_cast<const Dive::CreateSceneEvent&>(e);
	m_pActiveScene = const_cast<Dive::Scene*>(evt.GetCreatedScene());
}

void InspectorPanel::renderWindow()
{
	if (!m_pActiveScene)
		return;

	// ���� ScenePanel������ Selected�� �� �ִ�.
	if (!m_pEditor->GetHierarchy())
		return;

	auto pSelectedObject = m_pEditor->GetHierarchy()->GetSeletecedObject();
	if (!pSelectedObject)
		return;

	// draw components
	drawTransform(pSelectedObject);
	drawSpriteRenderer(pSelectedObject);
	drawMeshRenderer(pSelectedObject);
	drawLight(pSelectedObject);

	// add components
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		// ���� Component�� ������ �߰� ��ư �׸���

		ImGui::EndPopup();
	}
}

void InspectorPanel::drawTransform(Dive::GameObject* pSelectedObject)
{
	auto pTransform = pSelectedObject->GetComponent<Dive::Transform>();
	if (!pTransform)
		return;

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// position
		auto position = pTransform->GetLocalPosition();
		DrawVec3Control("Position", position);
		pTransform->SetLocalPosition(position);

		// rotation
		auto rotation = pTransform->GetLocalRotationDegrees();
		DrawVec3Control("Rotation", rotation);
		pTransform->SetLocalRotation(rotation);

		// scale
		auto scale = pTransform->GetLocalScale();
		DrawVec3Control("Scale", scale, 1.0f);
		pTransform->SetLocalScale(scale);
	}

	ImGui::Separator();
}

void InspectorPanel::drawSpriteRenderer(Dive::GameObject* pSelectedObject)
{
	auto pSpriteRenderer = pSelectedObject->GetComponent<Dive::SpriteRenderable>();
	if (!pSpriteRenderer)
		return;

	if (ImGui::CollapsingHeader("Sprite Renderable", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// enabled
		bool enabled = pSpriteRenderer->IsEnabled();
		ImGui::Checkbox("Enabled", &enabled);
		pSpriteRenderer->SetEnable(enabled);

		// Sprite
		//auto name = pSpriteRenderer->HasSprite() ? pSpriteRenderer->GetSprite()->GetName() : "";
		//ImGui::Text("Sprite");
		//ImGui::SameLine();
		//ImGui::InputText("##SpriteName", &name);
		//if (pSpriteRenderer->HasSprite())
		//	pSpriteRenderer->GetSprite()->SetName(name);

		//ImGui::SameLine();
		//if (ImGui::Button("*"))
		{
			// Asset/Textures ������ ����.
		}
		const char* items[] = { "", "Baseplate Grid", "ChoA", "dmc", "DokeV", "IU" };
		int currentItem = 0;
		ImGui::Text("Sprite");
		ImGui::SameLine();
		ImGui::Combo("##Sprite", &currentItem, items, IM_ARRAYSIZE(items));
		if(!pSpriteRenderer->HasSprite())
		{
			auto rscMng = Dive::ResourceManager::GetInstance();

			/*
			switch (currentItem)
			{
			case 0:
				pSpriteRenderer->SetSprite(nullptr);
				break;
			case 1:
				Dive::Texture2D::Create("Assets/Textures/Baseplate Grid.png");
				pSpriteRenderer->SetSprite(rscMng.GetResource<Dive::Texture2D>("Baseplate Grid"));
				break;
			case 2:
				Dive::Texture2D::Create("Assets/Textures/ChoA.jpg");
				pSpriteRenderer->SetSprite(rscMng.GetResource<Dive::Texture2D>("ChoA"));
				break;
			case 3:
				Dive::Texture2D::Create("Assets/Textures/dmc.jpg");
				pSpriteRenderer->SetSprite(rscMng.GetResource<Dive::Texture2D>("dmc"));
				break;
			case 4:
				Dive::Texture2D::Create("Assets/Textures/DokeV.jpeg");
				pSpriteRenderer->SetSprite(rscMng.GetResource<Dive::Texture2D>("DokeV"));
				break;
			case 5:
				Dive::Texture2D::Create("Assets/Textures/IU.jpg");
				pSpriteRenderer->SetSprite(rscMng.GetResource<Dive::Texture2D>("IU"));
				break;
			}
			*/
		}


		// Color
		DirectX::XMFLOAT4 color = pSpriteRenderer->GetColor();
		float col[4] = { color.x, color.y, color.z, color.w };
		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit4("##background", col);
		color = DirectX::XMFLOAT4(col[0], col[1], col[2], col[3]);
		pSpriteRenderer->SetColor(color);

		// Flip
		bool flipX = pSpriteRenderer->IsFlipX();
		bool flipY = pSpriteRenderer->IsFlipY();
		ImGui::Text("Flip");
		ImGui::SameLine();
		ImGui::Checkbox("X", &flipX);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &flipY);
		pSpriteRenderer->SetFlipX(flipX);
		pSpriteRenderer->SetFlipY(flipY);
	}
	
	ImGui::Separator();
}

void InspectorPanel::drawMeshRenderer(Dive::GameObject* pSelectedObject)
{
	auto pMeshRenderable = pSelectedObject->GetComponent<Dive::MeshRenderable>();
	if (!pMeshRenderable)
		return;

	if (ImGui::CollapsingHeader("Mesh Renderable", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// enabled
		bool enabled = pMeshRenderable->IsEnabled();
		ImGui::Checkbox("Enabled", &enabled);
		pMeshRenderable->SetEnable(enabled);


		// cast shadows

		// receive shadows

		if (ImGui::TreeNode("Materials"))
		{

			auto pMaterial = pMeshRenderable->GetMaterial();

			// Material�� ���´�. Ŭ���ϸ� �ϴܿ� ������ ���´�.
			// +, -�� ���ϰų� �� �� �ִ�.
			// Material�� ���� �� �ִ�. �� ��� �ش� ������Ʈ�� ���ֻ��� �ȴ�.
			ImGui::Text(pMaterial->GetName().c_str());

		// ���� MeshRenderable�� �ƴ� Material�� �����ְ� �ִ�.
		// �����δ� ������ �ϸ� ���� â�� ��� �����ִ���, ����Ƽó�� �Ʒ� ���ķ� ���� ���� ��� �Ѵ�. 

		// ���� �̰� ���� ���� �ִ�. �׷��� ��� �Ʒ��� ���� ���� ó���� �س��Ҵ�.
		// �׷��� ���� �̰� ������ �ȵ� �� �ִ�.
		// => Default Material�� ������ �ִ� ���� ���� �� ����.
			if (pMaterial)
			{
				// Rendering Mode(opaque)

				// albedo
				// �ܼ� ��ư�� ���� ������, �ƴϸ� ���� �ؽ��ĸ� ����ϴ��� Ȯ���� �ʿ��ϴ�.
				// => �巡�׸� ���� ����Ѵ�. �׸��� ���� �ؽ��ĸ� ����Ѵ�.
				if (ImGui::Button("*"))
				{
					auto path = Editor::FileOpen("Image Files (*.bmp;*.png;*.jpg;*.jpeg)\0*.bmp;*.png;*.jpg;*.jpeg");
					if (!path.empty())
					{
						const auto* pAlbedoTex = Dive::ResourceManager::GetInstance().Load<Dive::Texture2D>(path);
						// �̰͵� �Ʒ��� ������ ���� ���� ���δ�.
						pMaterial->SetMap(Dive::eMaterialMapType::Albedo, pAlbedoTex);
					}
				}
				ImGui::SameLine();
				ImGui::Text("Albedo");
				ImGui::SameLine();
				DirectX::XMFLOAT4 color = pMaterial->GetAlbedoColor();
				float col[4] = { color.x, color.y, color.z, color.w };
				ImGui::ColorEdit4("##background", col);
				color = DirectX::XMFLOAT4(col[0], col[1], col[2], col[3]);

				// metallic
				ImGui::Text("Metallic");

				// normal map
				ImGui::Text("Normal Map");

				// hight map
				ImGui::Text("Hight Map");

				// tiling
				auto tiling = pMaterial->GetTiling();
				ImGui::Text("Tiling");
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::PushItemWidth(128.0);	// ������ ���ϱ�?
				ImGui::SameLine();
				ImGui::InputFloat("##tilingX", &tiling.x, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::InputFloat("##tilingY", &tiling.y, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
				ImGui::PopItemWidth();


				// offset
				auto offset = pMaterial->GetOffset();
				ImGui::Text("Offset");
				ImGui::SameLine();
				ImGui::Text("X");
				ImGui::PushItemWidth(128.0);
				ImGui::SameLine();
				ImGui::InputFloat("##offsetX", &offset.x, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				ImGui::InputFloat("##offsetY", &offset.y, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
				ImGui::PopItemWidth();

				// ��Ƽ� �ϴ� ���� ���ƺ��δ�.
				pMaterial->SetAlbedoColor(color);
				pMaterial->SetTiling(tiling);	// ���� ������
				pMaterial->SetOffset(offset);	// ���� ������

				ImGui::TreePop();
			}
		}
	}

	ImGui::Separator();
}

void InspectorPanel::drawLight(Dive::GameObject* pSelectedObject)
{
	if (pSelectedObject == nullptr)
		return;

	auto pLightComponent = pSelectedObject->GetComponent<Dive::Light>();
	if (pLightComponent == nullptr)
		return;

	if (ImGui::CollapsingHeader("Light Renderable", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		// enabled
		bool enabled = pLightComponent->IsEnabled();
		ImGui::Checkbox("Enabled", &enabled);
		pLightComponent->SetEnable(enabled);

		// type
		const char* items[] = { "Directional", "Point", "Spot" };
		int currentItem = static_cast<int>(pLightComponent->GetLightType());
		ImGui::Text("Type");
		ImGui::SameLine();
		ImGui::Combo("##lightType", &currentItem, items, IM_ARRAYSIZE(items));
		pLightComponent->SetLightType(static_cast<Dive::eLightType>(currentItem));

		// color
		auto color = pLightComponent->GetColor();
		float col[3] = { color.x, color.y, color.z };
		ImGui::Text("Color");
		ImGui::SameLine();
		ImGui::ColorEdit3("##lightColor", col);
		color = DirectX::XMFLOAT3(col[0], col[1], col[2]);
		pLightComponent->SetColor(color);

		// range
		auto range = pLightComponent->GetRange();
		ImGui::Text("Range");
		ImGui::SameLine();
		ImGui::DragFloat("##Range", &range, 1.0f, 0.0f, 1000.0f, "%.2f");
		pLightComponent->SetRange(range);

		// spot angle
		auto spotAngle = pLightComponent->GetSpotAngle() * 2.0f;
		ImGui::Text("Spot Angle");
		ImGui::SameLine();
		ImGui::DragFloat("##SpotAngle", &spotAngle, 1.0f, 1.0f, 179.0f, "%.2f");
		pLightComponent->SetSpotAngle(spotAngle * 0.5f);
	}

	ImGui::Separator();
}
