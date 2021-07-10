#include "Hierarchy.h"
#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"

// �⺻ ���� 
// ���� ������Ʈ �߰��� ����Ʈ �̸��� id�� �־����� �Ѵ�.
// ����ȭ�� �ؾ� �Ѵ�.
// �������� �׽�Ʈ�� Ȯ���ϰ� �ؾ� �Ѵ�.

// ��Ʈ�� Ȯ��
// ���������� ������ �� �־�� �Ѵ�.
// GameObject�� �߰��� �� �־�� �Ѵ�.

// �ν����ͷ� Ȯ��
// inspector�� �����Ͱ� �ʿ��ϴ�.
// �������� ���� ������ �����ߴ�.


namespace editor
{
    Hierarchy::Hierarchy(Editor* editor)
        : Widget(editor)
    {
        mTitle = "Hierarchy";
        mFlags |= ImGuiWindowFlags_HorizontalScrollbar;
        m_Scene = nullptr;

        m_Selected = nullptr;
        m_Clicked = nullptr;
        m_Hovered = nullptr;
        m_Copied = nullptr;

        EVENT_SUBSCRIBE(dive::eEventType::SceneActivate, EVENT_HANDLE(OnSetActiveScene));
    }

    void Hierarchy::TickVisible()
    {
        if (!m_Scene)
            return;

        treeShow();

        if (ImGui::IsMouseReleased(0) && m_Clicked)
        {
            if (m_Hovered && m_Hovered->GetInstanceID() == m_Clicked->GetInstanceID())
            {
                setSelected(m_Clicked);
            }

            m_Clicked = nullptr;
        }
    }

    void Hierarchy::treeShow()
    {
        m_Hovered = nullptr;

        char sceneInfo[32] = { 0, };
        sprintf_s(sceneInfo, "%s - %d", m_Scene->GetName().c_str(), m_Scene->GetGameObjectCount());
        if (ImGui::TreeNodeEx(sceneInfo, ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (auto payload = DragDrop::GetInstance().GetPayload(eDragPayloadType::GameObject))
            {
                auto id = std::get<unsigned int>(payload->data);
                if (auto dropped = m_Scene->GetGameObjectByID(id))
                {
                    dropped->GetComponent<dive::Transform>()->SetParent(nullptr);
                }
            }

            auto rootGameObjects = m_Scene->GetRootGameObjects();
            for (auto gameObject : rootGameObjects)
            {
                treeAddGameObject(gameObject);
            }

            ImGui::TreePop();
        }

        handleClicking();

        popupPropertyMenu();
        popupGameObjectRename();
    }

    void Hierarchy::treeAddGameObject(dive::GameObject* gameObject)
    {
        if (!gameObject) return;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

        // �ڽ� ���� ����
        auto children = gameObject->GetComponent<dive::Transform>()->GetChildren();
        children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
       
        if (m_Selected)
        {
            nodeFlags |= (m_Selected->GetInstanceID() == gameObject->GetInstanceID()) ? ImGuiTreeNodeFlags_Selected : 0;
        }

        bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetInstanceID(), nodeFlags, gameObject->GetName().c_str());

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
        {
            m_Hovered = gameObject;
        }

        handleDragDrop(gameObject);

        if (bNodeOpen)
        {
            for (auto pChild : children)
            {
                treeAddGameObject(pChild->GetGameObject());
            }

            ImGui::TreePop();
        }
    }

    void Hierarchy::setSelected(dive::GameObject* gameObject)
    {
        // ���� �� �����ϸ� �ȵȴ�.
        //if (!gameObject)    return;

        m_Selected = gameObject;

        // �ν����Ϳ��� ����

    }

    void Hierarchy::handleClicking()
    {
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
            return;

        bool clickedLeft = ImGui::IsMouseClicked(0);
        bool clickedRight = ImGui::IsMouseClicked(1);

        if (clickedLeft)
        {
            if (m_Hovered)
                m_Clicked = m_Hovered;
            else
                setSelected(nullptr);
        }

        if (clickedRight)
            ImGui::OpenPopup("##PropertyMenu");
    }

    void Hierarchy::handleDragDrop(dive::GameObject* gameObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // �巡���� ��� ���� ������Ʈ�� ������ payload�� �����Ѵ�.
        if (dragDrop.DragBegin())
        {
            // ��� ������ �ʿ䰡...?
            m_Payload.data = gameObject->GetInstanceID();
            m_Payload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(m_Payload);
            dragDrop.DragEnd();
        }

        // ����� ��� ���� ������Ʈ�� Ȯ���ϰ�, ���� ������Ʈ�� �ٸ� ��� ���� ������Ʈ�� �θ�� �����Ѵ�.
        if (auto payload = dragDrop.GetPayload(eDragPayloadType::GameObject))
        {
            auto id = std::get<unsigned int>(payload->data);
            if (auto droppedObj = m_Scene->GetGameObjectByID(id))
            {
                if (droppedObj->GetInstanceID() != gameObject->GetInstanceID())
                {
                    droppedObj->GetComponent<dive::Transform>()->SetParent(
                        gameObject->GetComponent<dive::Transform>());
                }
            }
        }
    }

    void Hierarchy::popupPropertyMenu()
    {
        if (!ImGui::BeginPopup("##PropertyMenu"))
            return;

        if (ImGui::MenuItem("Copy", 0, false, m_Selected != nullptr))
        {
            m_Copied = m_Selected;
        }

        if (m_Copied)
        {
            if (ImGui::MenuItem("Paste", 0, false, m_Copied != nullptr))
            {
                //m_copied->Clone();
            }
        }

        ImGui::Separator();

        // ���� ������Ʈ�� ���õǾ� ���� �� Ȱ��ȭ
        if (ImGui::MenuItem("Rename", 0, false, m_Selected != nullptr))
        {
            m_bPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy���� �������� �˾ƾ� �Ѵ�.
        }

        if (ImGui::MenuItem("Remove", 0, false, m_Selected != nullptr))
        {
            m_Scene->RemoveGameObject(m_Selected);
            setSelected(nullptr);
        }

        ImGui::Separator();

        // ���õ� ����� ������ Ŭ���� ��
        // Component�� �߰���� �ϴ°� �´�.
        // �ϴ� ����Ƽ �������̽��� Ȯ������.

        if (ImGui::MenuItem("Create Empty"))
        {
            dive::Scene::GetGlobalScene().CreateGameObject();
        }

        if (ImGui::BeginMenu("3D Object"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("2D Object"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Effects"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Light"))
        {
            ImGui::EndMenu();
        }

        // audio
        // vidio
        // ui

        if (ImGui::MenuItem("Camera"))
        {
        }
        
        ImGui::EndPopup();
    }

    void Hierarchy::popupGameObjectRename()
    {
        if (m_bPopupRename)
        {
            ImGui::OpenPopup("##RenameGameObject");
            m_bPopupRename = false;
        }

        if (ImGui::BeginPopup("##RenameGameObject"))
        {
            auto pSelected = m_Selected;
            if (!pSelected)
            {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return;
            }

            auto name = pSelected->GetName();

            ImGui::Text("Name:");
            ImGui::InputText("##edit", &name);
            pSelected->SetName(std::string(name));

            if (ImGui::Button("Ok"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return;
            }

            ImGui::EndPopup();
        }
    }

    void Hierarchy::OnSetActiveScene()
    {
        m_Scene = dive::SceneManager::GetInstance().GetActiveScene();
    }
}