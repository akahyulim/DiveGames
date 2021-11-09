#include "Hierarchy.h"
#include "Inspector.h"
#include "External/ImGui/imgui_stdlib.h"
#include "Inspector.h"

using namespace DiveEngine;

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


namespace DiveEditor
{
    Hierarchy::Hierarchy(Editor* pEditor)
        : Widget(pEditor)
    {
        m_Title = "Hierarchy";
        m_Flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }

    // DragDrop으로 Transform을 어떻게 다룰 것인지 고심해야 한다.
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
    // Root의 순서 변경이 안된다.                      //
    // 아마 자식들 중 형제들 역시 마찬가지일 것이다.   //
    //=================================================//
    void Hierarchy::showMainTree()
    {
        assert(m_pScene);

        m_pHoveredGameObject = nullptr;

        char sceneInfo[32] = { 0, };
        sprintf_s(sceneInfo, "%s - %d", m_pScene->GetName().c_str(), m_pScene->GetGameObjectCount());

        // 이건 전체 트리다.
        if (ImGui::TreeNodeEx(sceneInfo, ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 빈 곳에 놓으면 독립
            // 현재 이 곳이 갱신되지 않고 있다.
            // 독립은 되는데 자식 관계도 여전히 보여진다.
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

    // 내부 트리를 만든다.
    void Hierarchy::showAddedTree(GameObject* gameObject)
    {
        if (!gameObject) return;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

        // 자식 존재 여부
        auto children = gameObject->GetComponent<Transform>()->GetChildren();
        children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
       
        if (m_pSelectedGameObject)
        {
            nodeFlags |= (m_pSelectedGameObject->GetInstanceID() == gameObject->GetInstanceID()) ? ImGuiTreeNodeFlags_Selected : 0;
        }

        // 이걸 굳이 if로 만들지 않은 이유는 무었일까?
        // 아래의 두 부분 때문인 것 같은데...
        bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)gameObject->GetInstanceID(), nodeFlags, gameObject->GetName().c_str());

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
        {
            m_pHoveredGameObject = gameObject;
        }

        // 드래그드랍 상태일 때 부모 대상이 되는 거다.
        // 결국 현재 이 부분에서도 버그가 발생되고 있다.
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

    // 드래그드랍을 처리한다.
    // 전달인자는 상황에 따라 다르게 사용된다.
    // 드래그일 땐 대상의 정보를 저장하고,
    // 드랍일 땐 저장된 대상의 부모로 설정한다.
    void Hierarchy::handleDragDrop(GameObject* gameObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // 드래그일 경우 현재 오브젝트의 정보를 payload에 저장한다.
        if (dragDrop.DragBegin())
        {
            // 아.. m_Payload를 멤버 변수로 만들 필요가 없구나.
            // 아닌가... 어딘가에 저장되어야 있어야 하나?
            m_Payload.data = gameObject->GetInstanceID();
            m_Payload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(m_Payload);
            dragDrop.DragEnd();

            APP_TRACE("Drag Target Name: {:s}, ID: {:d}", gameObject->GetName(), gameObject->GetInstanceID());
        }

        // 드랍일 경우 적재 오브젝트를 확인하고, 현재 오브젝트와 다를 경우 현재 오브젝트를 부모로 설정한다.
        if (auto payload = dragDrop.GetPayload(eDragPayloadType::GameObject))
        {
            auto id = std::get<unsigned int>(payload->data);
            if (auto droppedObj = m_pScene->GetGameObjectByID(id))
            {
                APP_TRACE("Payload Data id: {:d}", id);

                // 현재 이 놈이 안맞는다...
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
    // 몇몇 GameObject 구성 과정은 MenuBar와 동일하다.          //
    // 따라서 따로 함수화시킨 후 이를 사용하는 편이 직관적이다. //
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

                // 새로 생성된 GameObject들이 계속 하이라이트 상태다...
                // 아무래도 copy 본체의 selected 상태까지 가져온 듯 한데...
                // => 이게 문제가 아니다. 추가되는 모든 오브젝트가 동일한 결과다!!!!
            }
        }

        ImGui::Separator();

        // 게임 오브젝트가 선택되어 있을 때 활성화
        if (ImGui::MenuItem("Rename", 0, false, m_pSelectedGameObject != nullptr))
        {
            m_bPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy와의 차이점을 알아야 한다.
        }

        if (ImGui::MenuItem("Remove", 0, false, m_pSelectedGameObject != nullptr))
        {
            m_pScene->RemoveGameObject(m_pSelectedGameObject);
            setSelectedGameObject(nullptr);
        }

        ImGui::Separator();

        // 선택된 대상의 위에서 클릭할 땐
        // Component를 추가토록 하는게 맞다.
        // 일단 유니티 인테페이스를 확인하자.

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