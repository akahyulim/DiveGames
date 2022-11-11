#include "divepch.h"
#include "View.h"
#include "Viewport.h"
#include "Renderer.h"
#include "RenderPath.h"
#include "Mesh.h"
#include "Model.h"
#include "Core/Context.h"
#include "Core/EventSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/Texture2D.h"
#include "Resource/ResourceCache.h"
#include "Scene/Scene.h"
#include "Scene/Component/StaticModel.h"
#include "Scene/GameObject.h"
#include "IO/Log.h"
#include "Math/Math.h"

namespace Dive 
{
	View::View(Context* pContext)
		: Object(pContext)
	{
		m_pGraphics = GetSubsystem<Graphics>();
		m_pRenderer = GetSubsystem<Renderer>();
	}

	View::~View()
	{
		DV_LOG_ENGINE_DEBUG("Destroy View");
	}

	void View::Update(float delta)
	{
		// �����δ� octree�� cullCamera �̴�.
		if (!m_pScene)
			return;

		// GetDrawables()
		// �ϴ� �׷����� �� ������Ʈ���� �߸���.
		// ��Ʈ���� �ø��� ��ģ ������Ʈ�� �� �ϴ�.
		{
			m_Drawables.clear();

			auto allGameObjects = m_pScene->GetAllGameObjects();
			for (auto pGameObject : allGameObjects)
			{
				if (pGameObject->HasComponent<Dive::StaticModel>())
					m_Drawables.emplace_back(pGameObject->GetComponent<Dive::StaticModel>()->GetMesh(0));
			}
		}

		// GetBatches()
		// ������Ʈ�� �ϴ� ����Ʈ�� �Ϲ����� �и��Ѵ�.
		// �׸��� �׷����� ��Ŀ� ���� BatchGroup�� Batch�� ������ �����Ѵ�.
		{
			// Scene�� Component�� ������ ���� ���̹Ƿ�
			// Light�� Model Component�� �������� �� �� �ϴ�.
			// ������ vector ������ �ƴ϶� index�� ���� component ���ٸ� �����ϴ�.
			// ���� �̴� vector�� ���������ϴ� �Լ��� �߰��ϸ� �ȴ�.
			// urho�� octree component�� �����ϴ� �� �ϴ�.

			// spartan�� renderer���� object type�� entity vector�� �����Ѵ�.
			// �׸��� �� ������ scene���κ��� entity�� ������ �̸� �����ϸ�,
			// pass���� entity�� ���� component�� Ȯ���Ͽ� �׸���.
		}
	}
	
	void View::Render()
	{
		// clear
		{
			m_pCurrentRenderTarget = m_pRenderTarget;

			//eClearTarget flags = eClearTarget::Color | eClearTarget::Depth | eClearTarget::Stencil;
			m_pGraphics->SetRenderTarget(0, dynamic_cast<Texture2D*>(m_pCurrentRenderTarget));
			m_pGraphics->Clear(0, DirectX::XMFLOAT4(1.0f, 1.0f, 0.6f, 1.0f), 1.0f, 0);

			// �̰� �ӽ� ��ġ
			m_pGraphics->SetViewportRect(m_ViewRect);
		}

		// Draw
		{
			auto pDeviceContext = m_pGraphics->GetDeviceContext();

			for(const auto* pMesh : m_Drawables)
			{
				// set vertex buffer
				m_pGraphics->SetVertexBuffer(pMesh->GetVertexBuffer(0));
				
				// set index buffer
				m_pGraphics->SetIndexBuffer(pMesh->GetIndexBuffer());

				// set shaders & inputlayout
				m_pGraphics->SetDefaultShader();

				// draw
				// Mesh�� DrawRange�� �������� �ʾҴ�.
				auto indexCount = 3;// pMesh->GetIndexCount();
				auto indexStart = 0;// pMesh->GetIndexStart();
				auto primitiveType = pMesh->GetPrimitiveType();
				m_pGraphics->DrawIndexed(primitiveType, indexCount, indexStart);
			}
		}
	}
	
	bool View::Define(Texture* pRenderTarget, Viewport* pViewport)
	{
		m_pRenderTarget = pRenderTarget;

		// pViewport�κ��� RenderPath�� ȹ��
		m_pScene = pViewport->GetScene();
		// ī�޶� ȹ��

		int width = pRenderTarget ? pRenderTarget->GetWidth() : m_pGraphics->GetWidth();
		int height = pRenderTarget ? pRenderTarget->GetHeight() : m_pGraphics->GetHeight();
		m_RenderTargetSize = DirectX::XMINT2(width, height);

		auto rect = pViewport->GetRect();
		if (rect.left == 0 && rect.right == 0 && rect.top == 0 && rect.bottom == 0)
		{
			m_ViewRect.left = 0;
			m_ViewRect.right = width;
			m_ViewRect.top = 0;
			m_ViewRect.bottom = height;
		}
		else
		{
			m_ViewRect.left = Math::Clamp<int>(rect.left, 0, width - 1);
			m_ViewRect.right = Math::Clamp<int>(rect.right, m_ViewRect.left + 1, width);
			m_ViewRect.top = Math::Clamp<int>(rect.top, 0, height - 1);
			m_ViewRect.bottom = Math::Clamp<int>(rect.bottom, m_ViewRect.top + 1, height);
		}
		m_ViewSize = DirectX::XMINT2(m_ViewRect.right - m_ViewRect.left, m_ViewRect.bottom - m_ViewRect.top);

		return true;
	}
}