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

        // �ڽ� ���� ����
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
        // ���� �� �����ϸ� �ȵȴ�.
        //if (!gameObject)    return;

        mSelected = gameObject;

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

        // �巡���� ��� ���� ������Ʈ�� ������ payload�� �����Ѵ�.
        if (dragDrop.DragBegin())
        {
            // ��� ������ �ʿ䰡...?
            mPayload.data = gameObject->GetID();
            mPayload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(mPayload);
            dragDrop.DragEnd();
        }

        // ����� ��� ���� ������Ʈ�� Ȯ���ϰ�, ���� ������Ʈ�� �ٸ� ��� ���� ������Ʈ�� �θ�� �����Ѵ�.
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

        // ���� ������Ʈ�� ���õǾ� ���� �� Ȱ��ȭ
        if (ImGui::MenuItem("Rename", 0, false, mSelected != nullptr))
        {
            mbPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy���� �������� �˾ƾ� �Ѵ�.
        }

        if (ImGui::MenuItem("Remove", 0, false, mSelected != nullptr))
        {
            mScene->RemoveGameObject(mSelected);
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