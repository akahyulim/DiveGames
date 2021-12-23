#include "Hierarchy.h"
#include "Inspector.h"
#include "Geometry.h"
#include "External/ImGui/imgui_stdlib.h"

using namespace DiveEngine;

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

        if (ImGui::IsMouseReleased(0) && m_pClickedObject)
        {
            if (m_pHoveredObject && m_pHoveredObject->GetInstanceID() == m_pClickedObject->GetInstanceID())
            {
                setSelectedGameObject(m_pClickedObject);
            }

            m_pClickedObject = nullptr;
        }
    }

    //=================================================//
    // Root�� ���� ������ �ȵȴ�.                      //
    // �Ƹ� �ڽĵ� �� ������ ���� ���������� ���̴�.   //
    //=================================================//
    void Hierarchy::showMainTree()
    {
        assert(m_pScene);

        m_pHoveredObject = nullptr;

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
            for (auto pObject : rootGameObjects)
            {
                showAddedTree(pObject);
            }

            ImGui::TreePop();
        }

        handleClicking();

        popupPropertyMenu();
        popupGameObjectRename();
    }

    // ���� Ʈ���� �����.
    void Hierarchy::showAddedTree(GameObject* pObject)
    {
        if (!pObject) return;

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_AllowItemOverlap;

        // �ڽ� ���� ����
        auto children = pObject->GetComponent<Transform>()->GetChildren();
        children.empty() ? nodeFlags |= ImGuiTreeNodeFlags_Leaf : nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;
       
        if (m_pSelectedObject)
        {
            nodeFlags |= (m_pSelectedObject->GetInstanceID() == pObject->GetInstanceID()) ? ImGuiTreeNodeFlags_Selected : 0;
        }

        // �̰� ���� if�� ������ ���� ������ �����ϱ�?
        // �Ʒ��� �� �κ� ������ �� ������...
        bool bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)pObject->GetInstanceID(), nodeFlags, pObject->GetName().c_str());

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
        {
            m_pHoveredObject = pObject;
        }

        // �巡�׵�� ������ �� �θ� ����� �Ǵ� �Ŵ�.
        // �ᱹ ���� �� �κп����� ���װ� �߻��ǰ� �ִ�.
        handleDragDrop(pObject);

        if (bNodeOpen)
        {
            for (auto pChild : children)
            {
                showAddedTree(pChild->GetGameObject());
            }

            ImGui::TreePop();
        }
    }

    void Hierarchy::setSelectedGameObject(GameObject* pObject)
    {
        if (m_pSelectedObject != pObject)
        {
            m_pSelectedObject = pObject;
            Inspector::SetInspectGameObject(m_pSelectedObject);
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
            if (m_pHoveredObject)
                m_pClickedObject = m_pHoveredObject;
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
    void Hierarchy::handleDragDrop(GameObject* pObject)
    {
        auto dragDrop = DragDrop::GetInstance();

        // �巡���� ��� ���� ������Ʈ�� ������ payload�� �����Ѵ�.
        if (dragDrop.DragBegin())
        {
            // ��.. m_payload�� ��� ������ ���� �ʿ䰡 ������.
            // �ƴѰ�... ��򰡿� ����Ǿ�� �־�� �ϳ�?
            m_payload.data = pObject->GetInstanceID();
            m_payload.type = eDragPayloadType::GameObject;
            dragDrop.DragPayload(m_payload);
            dragDrop.DragEnd();

            APP_TRACE("Drag Target Name: {:s}, ID: {:d}", pObject->GetName(), pObject->GetInstanceID());
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

                if (droppedObj->GetInstanceID() != pObject->GetInstanceID())
                {
                    droppedObj->GetComponent<Transform>()->SetParent(
                        pObject->GetComponent<Transform>());

                    APP_TRACE("Drop Target Name: {:s}, ID: {:d}", pObject->GetName(), pObject->GetInstanceID());
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

        if (ImGui::MenuItem("Copy", 0, false, m_pSelectedObject != nullptr))
        {
            m_pCopiedObject = m_pSelectedObject;
        }

        if (m_pCopiedObject)
        {
            if (ImGui::MenuItem("Paste", 0, false, m_pCopiedObject != nullptr))
            {
                m_pCopiedObject->Duplicate();
            }
        }

        ImGui::Separator();

        // ���� ������Ʈ�� ���õǾ� ���� �� Ȱ��ȭ
        if (ImGui::MenuItem("Rename", 0, false, m_pSelectedObject != nullptr))
        {
            m_bPopupRename = true;
        }

        if (ImGui::MenuItem("Duplicate"))
        {
            // Copy���� �������� �˾ƾ� �Ѵ�.
        }

        if (ImGui::MenuItem("Remove", 0, false, m_pSelectedObject != nullptr))
        {
            m_pScene->RemoveGameObject(m_pSelectedObject);
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
          
            if (ImGui::MenuItem("Cube"))
            {
                static StaticMesh* pMesh = nullptr;

                if (!pMesh)
                {
                    std::vector<DiveEngine::Vertex_StaticMesh> vertices;
                    std::vector<uint32_t> indices;
                    Utility::CreateCube(vertices, indices);

                    pMesh = new DiveEngine::StaticMesh();
                    pMesh->SetVertices(vertices);
                    pMesh->SetIndices(indices);
                    pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
                    pMesh->SetName("Cube");
                }

                auto pGameObject = m_pScene->CreateGameObject();
                auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
                pMeshRenderer->SetMesh(pMesh);
                pGameObject->SetName(pMesh->GetName());
                
            }
          
            if (ImGui::MenuItem("Sphere"))
            {
                static StaticMesh* pMesh = nullptr;

                if (!pMesh)
                {
                    std::vector<DiveEngine::Vertex_StaticMesh> vertices;
                    std::vector<uint32_t> indices;
                    Utility::CreateSphere(vertices, indices);

                    pMesh = new DiveEngine::StaticMesh();
                    pMesh->SetVertices(vertices);
                    pMesh->SetIndices(indices);
                    pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
                    pMesh->SetName("Sphere");
                }

                auto pGameObject = m_pScene->CreateGameObject();
                auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
                pMeshRenderer->SetMesh(pMesh);
                pGameObject->SetName(pMesh->GetName());
            }

            if (ImGui::MenuItem("Capsule"))
            {
            }
            
            if (ImGui::MenuItem("Cylinder"))
            {
                static StaticMesh* pMesh = nullptr;

                if (!pMesh)
                {
                    std::vector<DiveEngine::Vertex_StaticMesh> vertices;
                    std::vector<uint32_t> indices;
                    Utility::CreateCylinder(vertices, indices);

                    pMesh = new DiveEngine::StaticMesh();
                    pMesh->SetVertices(vertices);
                    pMesh->SetIndices(indices);
                    pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
                    pMesh->SetName("Cylinder");
                }

                auto pGameObject = m_pScene->CreateGameObject();
                auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
                pMeshRenderer->SetMesh(pMesh);
                pGameObject->SetName(pMesh->GetName());
            }

            if (ImGui::MenuItem("Plane"))
            {
                static StaticMesh* pMesh = nullptr;

                if (!pMesh)
                {
                    std::vector<DiveEngine::Vertex_StaticMesh> vertices;
                    std::vector<uint32_t> indices;
                    Utility::CreatePlane(vertices, indices);

                    pMesh = new DiveEngine::StaticMesh();
                    pMesh->SetVertices(vertices);
                    pMesh->SetIndices(indices);
                    pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
                    pMesh->SetName("Plane");
                }

                auto pGameObject = m_pScene->CreateGameObject();
                auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
                pMeshRenderer->SetMesh(pMesh);
                pGameObject->SetName(pMesh->GetName());
            }

            if (ImGui::MenuItem("Quad"))
            {
                static StaticMesh* pMesh = nullptr;

                if (!pMesh)
                {
                    std::vector<DiveEngine::Vertex_StaticMesh> vertices;
                    std::vector<uint32_t> indices;
                    Utility::CreateQuad(vertices, indices);

                    pMesh = new DiveEngine::StaticMesh();
                    pMesh->SetVertices(vertices);
                    pMesh->SetIndices(indices);
                    pMesh->CreateBuffers(Renderer::GetInstance().GetGraphicsDevice()->GetDevice());
                    pMesh->SetName("Quad");
                }

                auto pGameObject = m_pScene->CreateGameObject();
                auto pMeshRenderer = pGameObject->AddComponent<StaticMeshRenderer>();
                pMeshRenderer->SetMesh(pMesh);
                pGameObject->SetName(pMesh->GetName());
            }

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
            auto pSelected = m_pSelectedObject;
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