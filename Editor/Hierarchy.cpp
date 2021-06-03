#include "Hierarchy.h"
#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"

// 기본 구현 
// 게임 오브젝트 추가시 디폴트 이름에 id를 넣었으면 한다.
// 파일화를 해야 한다.
// 계층구조 테스트를 확실하게 해야 한다.

// 컨트롤 확장
// 계층구조를 제어할 수 있어야 한다.
// GameObject를 추가할 수 있어야 한다.

// 인스펙터로 확장
// inspector의 포인터가 필요하다.
// 예전에는 전역 변수로 접근했다.


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

        // 자식 존재 여부
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
        if (!pGameObject)    return;

        m_pSelected = pGameObject;

        // 인스펙터에도 전달

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
        {
            if (m_pHovered)
            {
                setSelected(m_pHovered);
                ImGui::OpenPopup("##PropertyMenu");
            }
            else
            {
                setSelected(nullptr);
                ImGui::OpenPopup("##HierarchyGameObjectMenu");
            }
        }
    }

    void Hierarchy::handleDragDrop(Dive::GameObject* pGameObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // 드래그일 경우 현재 오브젝트의 정보를 payload에 저장한다.
        if (dragDrop.DragBegin())
        {
            // 멤버 변수일 필요가...?
            m_payload.data = pGameObject->GetID();
            m_payload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(m_payload);
            dragDrop.DragEnd();
        }

        // 드랍일 경우 적재 오브젝트를 확인하고, 현재 오브젝트와 다를 경우 현재 오브젝트를 부모로 설정한다.
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

        //if (ImGui::MenuItem(u8"Copy", "Ctrl + C"))
        {
        //    m_pCopied = m_pSelected;
        }

        // 구현하지 않았다.
        //if (m_pCopied)
        {
        //    if (ImGui::MenuItem(u8"Paste", "Ctrl + V"))
            {
                //m_copied->Clone();
            }
        }

        ImGui::Separator();

        // 일단 잘 돌아간다. 자식들까지 전부 제거한다.
        if (ImGui::MenuItem(u8"Remove", "Del"))
        {
            m_pScene->RemoveGameObject(m_pSelected);
            setSelected(nullptr);
        }

        if (ImGui::MenuItem(u8"Rename", "F2"))
        {
            m_bPopupRename = true;
        }

        ImGui::Separator();

        // GameObject 생성 함수들을 처리한다.
        // 일단 유니티의 인터페이스를 확인한 후 구현하자.

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