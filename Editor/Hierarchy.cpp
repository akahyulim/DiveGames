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


namespace editor
{
    Hierarchy::Hierarchy(Editor* editor)
        : Widget(editor)
    {
        mTitle = "Hierarchy";
        mFlags |= ImGuiWindowFlags_HorizontalScrollbar;
        mScene = &dive::Scene::GetGlobalScene();

        mSelected = nullptr;
        mClicked = nullptr;
        mHovered = nullptr;
        mCopied = nullptr;
    }

    void Hierarchy::TickVisible()
    {
        treeShow();

        if (ImGui::IsMouseReleased(0) && mClicked)
        {
            if (mHovered && mHovered->GetID() == mClicked->GetID())
            {
                setSelected(mClicked);
            }

            mClicked = nullptr;
        }
    }

    void Hierarchy::treeShow()
    {
        mHovered = nullptr;

        char sceneInfo[32] = { 0, };
        sprintf_s(sceneInfo, "%s - %d", mScene->GetName().c_str(), mScene->GetGameObjectCount());
        if (ImGui::TreeNodeEx(sceneInfo, ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (auto payload = DragDrop::GetInstance().GetPayload(eDragPayloadType::GameObject))
            {
                auto id = std::get<unsigned int>(payload->data);
                if (auto dropped = mScene->GetGameObjectByID(id))
                {
                    dropped->GetComponent<dive::Transform>()->SetParent(nullptr);
                }
            }

            auto rootGameObjects = mScene->GetRootGameObjects();
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

        // 자식 존재 여부
        auto children = gameObject->GetComponent<dive::Transform>()->GetChildren();
        children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
       
        if (mSelected)
        {
            nodeFlags |= (mSelected->GetID() == gameObject->GetID()) ? ImGuiTreeNodeFlags_Selected : 0;
        }

        bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetID(), nodeFlags, gameObject->GetName().c_str());

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
        {
            mHovered = gameObject;
        }

        handleDragDrop(gameObject);

        if (bNodeOpen)
        {
            for (auto pChild : children)
            {
                treeAddGameObject(pChild->GetOwner());
            }

            ImGui::TreePop();
        }
    }

    void Hierarchy::setSelected(dive::GameObject* gameObject)
    {
        // 없을 때 리턴하면 안된다.
        //if (!gameObject)    return;

        mSelected = gameObject;

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
            if (mHovered)
                mClicked = mHovered;
            else
                setSelected(nullptr);
        }

        if (clickedRight)
            ImGui::OpenPopup("##PropertyMenu");
    }

    void Hierarchy::handleDragDrop(dive::GameObject* gameObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // 드래그일 경우 현재 오브젝트의 정보를 payload에 저장한다.
        if (dragDrop.DragBegin())
        {
            // 멤버 변수일 필요가...?
            mPayload.data = gameObject->GetID();
            mPayload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(mPayload);
            dragDrop.DragEnd();
        }

        // 드랍일 경우 적재 오브젝트를 확인하고, 현재 오브젝트와 다를 경우 현재 오브젝트를 부모로 설정한다.
        if (auto payload = dragDrop.GetPayload(eDragPayloadType::GameObject))
        {
            auto id = std::get<unsigned int>(payload->data);
            if (auto droppedObj = mScene->GetGameObjectByID(id))
            {
                if (droppedObj->GetID() != gameObject->GetID())
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

        if (ImGui::MenuItem("Copy", 0, false, mSelected != nullptr))
        {
            mCopied = mSelected;
        }

        if (mCopied)
        {
            if (ImGui::MenuItem("Paste", 0, false, mCopied != nullptr))
            {
                //m_copied->Clone();
            }
        }

        ImGui::Separator();

        // 게임 오브젝트가 선택되어 있을 때 활성화
        if (ImGui::MenuItem("Rename", 0, false, mSelected != nullptr))
        {
            mbPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy와의 차이점을 알아야 한다.
        }

        if (ImGui::MenuItem("Remove", 0, false, mSelected != nullptr))
        {
            mScene->RemoveGameObject(mSelected);
            setSelected(nullptr);
        }

        ImGui::Separator();

        // 선택된 대상의 위에서 클릭할 땐
        // Component를 추가토록 하는게 맞다.
        // 일단 유니티 인테페이스를 확인하자.

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
        if (mbPopupRename)
        {
            ImGui::OpenPopup("##RenameGameObject");
            mbPopupRename = false;
        }

        if (ImGui::BeginPopup("##RenameGameObject"))
        {
            auto pSelected = mSelected;
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