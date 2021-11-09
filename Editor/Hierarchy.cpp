#include "Hierarchy.h"
#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"
#include "Inspector.h"

using namespace DiveEngine;

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


namespace DiveEditor
{
    Hierarchy::Hierarchy(Editor* pEditor)
        : Widget(pEditor)
    {
        m_Title = "Hierarchy";
        m_Flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    // DragDrop���� Transform�� ��� �ٷ� ������ ����ؾ� �Ѵ�.
    void Hierarchy::TickVisible()
    {
        if (!m_pScene)
            return;

        showMainTree();

        if (ImGui::IsMouseReleased(0) && m_pClickedGameObject)
        {
            if (m_pHoveredGameObject && m_pHoveredGameObject->GetInstanceID() == m_pClickedGameObject->GetInstanceID())
            {
                setSelectedGameObject(m_pClickedGameObject);
            }

            m_pClickedGameObject = nullptr;
        }
    }

    //=================================================//
    // Root�� ���� ������ �ȵȴ�.                      //
    // �Ƹ� �ڽĵ� �� ������ ���� ���������� ���̴�.   //
    //=================================================//
    void Hierarchy::showMainTree()
    {
        assert(m_pScene);

        m_pHoveredGameObject = nullptr;

        char sceneInfo[32] = { 0, };
        sprintf_s(sceneInfo, "%s - %d", m_pScene->GetName().c_str(), m_pScene->GetGameObjectCount());

        // �̰� ��ü Ʈ����.
        if (ImGui::TreeNodeEx(sceneInfo, ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �� ���� ������ ����
            // ���� �� ���� ���ŵ��� �ʰ� �ִ�.
            // ������ �Ǵµ� �ڽ� ���赵 ������ ��������.
            if (auto payload = DragDrop::GetInstance().GetPayload(eDragPayloadType::GameObject))
            {
                auto id = std::get<unsigned int>(payload->data);
                if (auto dropped = m_pScene->GetGameObjectByID(id))
                {
                    dropped->GetComponent<Transform>()->SetParent(nullptr);
                }
            }

            auto rootGameObjects = m_pScene->GetRootGameObjects();
            for (auto gameObject : rootGameObjects)
            {
                showAddedTree(gameObject);
            }

            ImGui::TreePop();
        }

        handleClicking();

        popupPropertyMenu();
        popupGameObjectRename();
    }

    // ���� Ʈ���� �����.
    void Hierarchy::showAddedTree(GameObject* gameObject)
    {
        if (!gameObject) return;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

        // �ڽ� ���� ����
        auto children = gameObject->GetComponent<Transform>()->GetChildren();
        children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
       
        if (m_pSelectedGameObject)
        {
            nodeFlags |= (m_pSelectedGameObject->GetInstanceID() == gameObject->GetInstanceID()) ? ImGuiTreeNodeFlags_Selected : 0;
        }

        // �̰� ���� if�� ������ ���� ������ �����ϱ�?
        // �Ʒ��� �� �κ� ������ �� ������...
        bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetInstanceID(), nodeFlags, gameObject->GetName().c_str());

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
        {
            m_pHoveredGameObject = gameObject;
        }

        // �巡�׵�� ������ �� �θ� ����� �Ǵ� �Ŵ�.
        // �ᱹ ���� �� �κп����� ���װ� �߻��ǰ� �ִ�.
        handleDragDrop(gameObject);

        if (bNodeOpen)
        {
            for (auto pChild : children)
            {
                showAddedTree(pChild->GetGameObject());
            }

            ImGui::TreePop();
        }
    }

    void Hierarchy::setSelectedGameObject(GameObject* gameObject)
    {
        if (m_pSelectedGameObject != gameObject)
        {
            m_pSelectedGameObject = gameObject;
            Inspector::SetInspectGameObject(m_pSelectedGameObject);
        }
    }

    void Hierarchy::handleClicking()
    {
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
            return;

        bool clickedLeft = ImGui::IsMouseClicked(0);
        bool clickedRight = ImGui::IsMouseClicked(1);

        if (clickedLeft)
        {
            if (m_pHoveredGameObject)
                m_pClickedGameObject = m_pHoveredGameObject;
            else
                setSelectedGameObject(nullptr);
        }

        if (clickedRight)
            ImGui::OpenPopup("##PropertyMenu");
    }

    // �巡�׵���� ó���Ѵ�.
    // �������ڴ� ��Ȳ�� ���� �ٸ��� ���ȴ�.
    // �巡���� �� ����� ������ �����ϰ�,
    // ����� �� ����� ����� �θ�� �����Ѵ�.
    void Hierarchy::handleDragDrop(GameObject* gameObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // �巡���� ��� ���� ������Ʈ�� ������ payload�� �����Ѵ�.
        if (dragDrop.DragBegin())
        {
            // ��.. m_Payload�� ��� ������ ���� �ʿ䰡 ������.
            // �ƴѰ�... ��򰡿� ����Ǿ�� �־�� �ϳ�?
            m_Payload.data = gameObject->GetInstanceID();
            m_Payload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(m_Payload);
            dragDrop.DragEnd();

            APP_TRACE("Drag Target Name: {:s}, ID: {:d}", gameObject->GetName(), gameObject->GetInstanceID());
        }

        // ����� ��� ���� ������Ʈ�� Ȯ���ϰ�, ���� ������Ʈ�� �ٸ� ��� ���� ������Ʈ�� �θ�� �����Ѵ�.
        if (auto payload = dragDrop.GetPayload(eDragPayloadType::GameObject))
        {
            auto id = std::get<unsigned int>(payload->data);
            if (auto droppedObj = m_pScene->GetGameObjectByID(id))
            {
                APP_TRACE("Payload Data id: {:d}", id);

                // ���� �� ���� �ȸ´´�...
                APP_TRACE("Drop Payload Name: {:s}, ID: {:d}", droppedObj->GetName(), droppedObj->GetInstanceID());

                if (droppedObj->GetInstanceID() != gameObject->GetInstanceID())
                {
                    droppedObj->GetComponent<Transform>()->SetParent(
                        gameObject->GetComponent<Transform>());

                    APP_TRACE("Drop Target Name: {:s}, ID: {:d}", gameObject->GetName(), gameObject->GetInstanceID());
                }
            }
        }
    }

    //==========================================================//
    // ��� GameObject ���� ������ MenuBar�� �����ϴ�.          //
    // ���� ���� �Լ�ȭ��Ų �� �̸� ����ϴ� ���� �������̴�. //
    //==========================================================//
    void Hierarchy::popupPropertyMenu()
    {
        if (!ImGui::BeginPopup("##PropertyMenu"))
            return;

        if (ImGui::MenuItem("Copy", 0, false, m_pSelectedGameObject != nullptr))
        {
            m_pCopiedGameObject = m_pSelectedGameObject;
        }

        if (m_pCopiedGameObject)
        {
            if (ImGui::MenuItem("Paste", 0, false, m_pCopiedGameObject != nullptr))
            {
                m_pCopiedGameObject->Duplicate();

                // ���� ������ GameObject���� ��� ���̶���Ʈ ���´�...
                // �ƹ����� copy ��ü�� selected ���±��� ������ �� �ѵ�...
                // => �̰� ������ �ƴϴ�. �߰��Ǵ� ��� ������Ʈ�� ������ �����!!!!
            }
        }

        ImGui::Separator();

        // ���� ������Ʈ�� ���õǾ� ���� �� Ȱ��ȭ
        if (ImGui::MenuItem("Rename", 0, false, m_pSelectedGameObject != nullptr))
        {
            m_bPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy���� �������� �˾ƾ� �Ѵ�.
        }

        if (ImGui::MenuItem("Remove", 0, false, m_pSelectedGameObject != nullptr))
        {
            m_pScene->RemoveGameObject(m_pSelectedGameObject);
            setSelectedGameObject(nullptr);
        }

        ImGui::Separator();

        // ���õ� ����� ������ Ŭ���� ��
        // Component�� �߰���� �ϴ°� �´�.
        // �ϴ� ����Ƽ �������̽��� Ȯ������.

        if (ImGui::MenuItem("Create Empty"))
        {
            if (m_pScene)
            {
                m_pScene->CreateGameObject();
            }
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
            auto pSelected = m_pSelectedGameObject;
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