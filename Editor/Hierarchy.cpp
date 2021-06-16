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


namespace Editor
{
    Hierarchy::Hierarchy(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Hierarchy";
        m_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        m_pScene = &Dive::Scene::GetGlobalScene();

        m_pSelected = nullptr;
        m_pClicked = nullptr;
        m_pHovered = nullptr;
        m_pCopied = nullptr;
    }

    void Hierarchy::TickVisible()
    {
        treeShow();

        if (ImGui::IsMouseReleased(0) && m_pClicked)
        {
            if (m_pHovered && m_pHovered->GetID() == m_pClicked->GetID())
            {
                setSelected(m_pClicked);
            }

            m_pClicked = nullptr;
        }
    }

    void Hierarchy::treeShow()
    {
        m_pHovered = nullptr;

        char sceneInfo[32] = { 0, };
        sprintf_s(sceneInfo, "%s - %d", m_pScene->GetName().c_str(), m_pScene->GetGameObjectCount());
        if (ImGui::TreeNodeEx(sceneInfo, ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (auto payload = DragDrop::GetInstance().GetPayload(eDragPayloadType::GameObject))
            {
                auto id = std::get<unsigned int>(payload->data);
                if (auto dropped = m_pScene->GetGameObjectByID(id))
                {
                    dropped->GetComponent<Dive::Transform>()->SetParent(nullptr);
                }
            }

            auto rootGameObjects = m_pScene->GetRootGameObjects();
            for (auto pGameObject : rootGameObjects)
            {
                treeAddGameObject(pGameObject);
            }

            ImGui::TreePop();
        }

        handleClicking();

        popupPropertyMenu();
        popupGameObjectRename();
    }

    void Hierarchy::treeAddGameObject(Dive::GameObject* pGameObject)
    {
        if (!pGameObject) return;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

        // �ڽ� ���� ����
        auto children = pGameObject->GetComponent<Dive::Transform>()->GetChildren();
        children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
       
        if (m_pSelected)
        {
            nodeFlags |= (m_pSelected->GetID() == pGameObject->GetID()) ? ImGuiTreeNodeFlags_Selected : 0;
        }

        bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)pGameObject->GetID(), nodeFlags, pGameObject->GetName().c_str());

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
        {
            m_pHovered = pGameObject;
        }

        handleDragDrop(pGameObject);

        if (bNodeOpen)
        {
            for (auto pChild : children)
            {
                treeAddGameObject(pChild->GetOwner());
            }

            ImGui::TreePop();
        }
    }

    void Hierarchy::setSelected(Dive::GameObject* pGameObject)
    {
        // ���� �� �����ϸ� �ȵȴ�.
        //if (!pGameObject)    return;

        m_pSelected = pGameObject;

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
            if (m_pHovered)
                m_pClicked = m_pHovered;
            else
                setSelected(nullptr);
        }

        if (clickedRight)
            ImGui::OpenPopup("##PropertyMenu");
    }

    void Hierarchy::handleDragDrop(Dive::GameObject* pGameObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // �巡���� ��� ���� ������Ʈ�� ������ payload�� �����Ѵ�.
        if (dragDrop.DragBegin())
        {
            // ��� ������ �ʿ䰡...?
            m_payload.data = pGameObject->GetID();
            m_payload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(m_payload);
            dragDrop.DragEnd();
        }

        // ����� ��� ���� ������Ʈ�� Ȯ���ϰ�, ���� ������Ʈ�� �ٸ� ��� ���� ������Ʈ�� �θ�� �����Ѵ�.
        if (auto payload = dragDrop.GetPayload(eDragPayloadType::GameObject))
        {
            auto id = std::get<unsigned int>(payload->data);
            if (auto droppedObj = m_pScene->GetGameObjectByID(id))
            {
                if (droppedObj->GetID() != pGameObject->GetID())
                {
                    droppedObj->GetComponent<Dive::Transform>()->SetParent(
                        pGameObject->GetComponent<Dive::Transform>());
                }
            }
        }
    }

    void Hierarchy::popupPropertyMenu()
    {
        if (!ImGui::BeginPopup("##PropertyMenu"))
            return;

        if (ImGui::MenuItem("Copy", 0, false, m_pSelected != nullptr))
        {
            m_pCopied = m_pSelected;
        }

        if (m_pCopied)
        {
            if (ImGui::MenuItem("Paste", 0, false, m_pCopied != nullptr))
            {
                //m_copied->Clone();
            }
        }

        ImGui::Separator();

        // ���� ������Ʈ�� ���õǾ� ���� �� Ȱ��ȭ
        if (ImGui::MenuItem("Rename", 0, false, m_pSelected != nullptr))
        {
            m_bPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy���� �������� �˾ƾ� �Ѵ�.
        }

        if (ImGui::MenuItem("Remove", 0, false, m_pSelected != nullptr))
        {
            m_pScene->RemoveGameObject(m_pSelected);
            setSelected(nullptr);
        }

        ImGui::Separator();

        // ���õ� ����� ������ Ŭ���� ��
        // Component�� �߰���� �ϴ°� �´�.
        // �ϴ� ����Ƽ �������̽��� Ȯ������.

        if (ImGui::MenuItem("Create Empty"))
        {
            Dive::Scene::GetGlobalScene().CreateGameObject();
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
            auto pSelected = m_pSelected;
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
}