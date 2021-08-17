#include "Renderer.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "GameObject.h"
#include "Log.h"
#include "TextMesh.h"
#include "Event.h"
#include "Camera.h"
#include "DiveCore.h"
#include <assert.h>

namespace dive
{
	Renderer::Renderer()
	{
		m_RenderTargetSize = DirectX::XMINT2(0, 0);
		// �̷��� �ʱ�ȭ�ϸ� Sandbox���� ������ �����.
		m_Viewport.Width = static_cast<float>(m_RenderTargetSize.x);
		m_Viewport.Height = static_cast<float>(m_RenderTargetSize.y);
		//m_Viewport.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		//m_Viewport.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;

		EVENT_SUBSCRIBE(eEventType::SceneResolve, EVENT_HANDLE(ObjectClassify));
	}

	Renderer::~Renderer()
	{
		DV_DELETE(m_pGraphicsDevice);

		// ���ҽ� ����
	}

	void Renderer::Initialize(HWND hWnd, bool fullScreen)
	{
		// �ϴ� ����
		m_pGraphicsDevice = new GraphicsDevice(hWnd, fullScreen);

		if (!m_pGraphicsDevice || !m_pGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("Graphics Device�� �������� �ʾ� �ʱ�ȭ�� ������ �� �����ϴ�. ���α׷��� �����մϴ�.");
			PostQuitMessage(0);
		}

		// ���°� ��... �ϰ����� ����.
		m_pGBuffer = new GBuffer(m_pGraphicsDevice->GetDevice());
		// ũ�Ⱑ �̷��� �ȵȴ�. �Ϲ������� ������ Editor�� Scene���� �ٸ���.
		// ���� ������ ���� RenderPath3D�̴�. �׷��� RenderPath�� ���� �м��� �ȵǾ��ִ�.
		// �ܺο��� ���� �Ʒ� �Լ��� ȣ����� �ؾ� �ϴ� �ɱ�...?
		// �ƴϸ� �ϴ��� ���⿡�� �ʱ�ȭ �������� �����س���
		// Editor�� Scene���� �ٽ� �����ϴ°ɷ�ٰ�...
		m_pGBuffer->Initialize(m_pGraphicsDevice->GetResolutionHeight(), m_pGraphicsDevice->GetResolutionHeight());

		// states
		createDepthStencilStates();
		createRasterizerStates();
		createSamplerStates();

		createConstantBuffers();
		//createTextures();				// ����� Sandbox �׽�Ʈ��, Spartan�� Editor���� ����ϴ� Texture ������
		createRenderTargets();		// ���� RenderTexture���� �����.
		createShaders();

		createFonts();

		createPipelineStates();	// ���� �������̾�� �Ѵ�.
		
		CORE_TRACE("Renderer �ʱ�ȭ�� �����Ͽ����ϴ�.");
	}

	void Renderer::Update(float deltaTime)
	{
		// update

		// render
	}

	//==============================================================================//
	// �̰� ���ٴ�. ���� �� ���� ������ Sandbox�� ������� �ʴ´�.					//
	// Camera�� �ڽ��� �׷��� RenderTarget�� ũ�⸦ ������ ã�Ƽ� �����ؾ� �Ѵ�.	//
	// �װ� �ƴϸ� �ᱹ ��ó�� ũ�Ⱑ �޶��� �� ���� Camera���� �˷���� �Ѵ�.		//
	//==============================================================================//
	void Renderer::SetViewport(float width, float height, float offsetX, float offsetY)
	{
		if (m_pCamera)
		{
			auto pCamera = m_pCamera->GetComponent<Camera>();
			pCamera->SetScreenSize(width, height);
		}
		//if (m_Viewport.Width != width || m_Viewport.Height != height)
		else
		{
			// ������ �ϴµ�...

			m_Viewport.Width = width;
			m_Viewport.Height = height;

			// ���ʿ� offset�� ����...
		}
	}

	// �ٽ� �� �� �������� RenderTarget�� ũ�⼳���̴�.
	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		if (m_RenderTargetSize.x == width && m_RenderTargetSize.y == height)
			return;

		// �̿ܿ��� ũ�Ⱑ �´��� Ȯ���� �ʿ��ϴ�.

		m_RenderTargetSize.x = width;
		m_RenderTargetSize.y = height;

		createRenderTargets();

		CORE_TRACE("�ػ� ����: {0:d}x{1:d}", width, height);
	}

	void Renderer::GetGBufferSize(unsigned int& outWidth, unsigned int& outHeight)
	{
		if (!m_pGBuffer)
		{
			outWidth = 0, outHeight = 0;
		}
		else
		{
			outWidth = m_pGBuffer->GetWidth();
			outHeight = m_pGBuffer->GetHeight();
		}
	}

	//======================================================================================//
	// ���Ÿ� �Ѵ�. bind�� ���� path���� �Ѵ�.												//
	// ���� wciked�� spartan�� ������ ������.												//
	// Main Camera�� ���� View, Projection�� �̿��� ����� �����							//
	// �� ������ �����ϴ� Constant Buffer�� �����͸� ����(CPU)�Ѵ�.							//
	// ���� ������ Constant Buffer�� �ϵ� �ڵ��� View, Projection�� ���Ǿ�				//
	// �����ϴ�.																			//
	//======================================================================================//
	void Renderer::UpdateCB()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		if (m_pBufferFrame == nullptr)
			return;

		MatrixBuffer* pBuffer = static_cast<MatrixBuffer*>(m_pBufferFrame->Map());

		// World Matrix
		// �̰� ���� GameObject�� Transform���κ��� �����´�.
		// �ʿ���� �� ����. ���� ������ ���Žô�.
		pBuffer->world = XMMatrixTranspose(XMMatrixIdentity());

		// View�� Projection�� Camera�κ��� �����´�.
		// => ī�޶� �������� �� �ִ�.
		// => Scene Ȥ�� Camera�� static���κ��� Ȱ��ȭ�� ī�޶� ��� ���;� �Ѵ�.
		// => �׷��� �Ǹ� pass�� �������� �Ͽ�ư ����������...
		if (m_pCamera)
		{
			auto pCamera = m_pCamera->GetComponent<Camera>();

			// view
			auto viewMatrix = pCamera->GetViewMatrix();
			pBuffer->view = XMMatrixTranspose(viewMatrix);

			// projection
			auto projMatrix = pCamera->GetProjectionMatrix();
			pBuffer->proj = XMMatrixTranspose(projMatrix);

			// �� ���̶� �־�� �ϳ�??? �ƴϸ� Projection�� �ϳ��� ������� �ؾ� �ϳ�...
			// �װ͵� �ƴϸ� �����ζ� ����ؼ� �� �� �� �־�� �ϳ�...
			// orthographic
			auto orthoProjMatrix = XMMatrixTranspose(DirectX::XMMatrixIdentity());
			pBuffer->projOrthographic = orthoProjMatrix;

			// constant buffer
			m_BufferFrameCPU.SetViewMatrix(viewMatrix);
			m_BufferFrameCPU.SetPerspectiveProjectionMatrix(projMatrix);
			m_BufferFrameCPU.SetViewProjectionMatrix(viewMatrix * projMatrix);//pCamera->GetViewProjectionMatrix());
			m_BufferFrameCPU.SetOrthoProjectionMatrix(orthoProjMatrix);
		}
		else
		{
			// View Matrix
			{
				XMFLOAT3 up, position, lookAt;
				XMVECTOR upVector, positionVector, lookAtVector;
				float yaw, pitch, roll;
				XMMATRIX rotationMatrix;

				up.x = 0.0f;
				up.y = 1.0f;
				up.z = 0.0f;

				upVector = XMLoadFloat3(&up);

				position.x = 0.0f;
				position.y = 0.0f;
				position.z = -5.0f;

				// Load it into a XMVECTOR structure.
				positionVector = XMLoadFloat3(&position);

				// Setup where the camera is looking by default.
				lookAt.x = 0.0f;
				lookAt.y = 0.0f;
				lookAt.z = 1.0f;

				// Load it into a XMVECTOR structure.
				lookAtVector = XMLoadFloat3(&lookAt);

				// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
				pitch = 0.0f * 0.0174532925f;
				yaw = 0.0f * 0.0174532925f;
				roll = 0.0f * 0.0174532925f;

				// Create the rotation matrix from the yaw, pitch, and roll values.
				rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

				// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
				lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
				upVector = XMVector3TransformCoord(upVector, rotationMatrix);

				// Translate the rotated camera position to the location of the viewer.
				lookAtVector = XMVectorAdd(positionVector, lookAtVector);

				auto viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
				pBuffer->view = XMMatrixTranspose(viewMatrix);
				m_BufferFrameCPU.SetViewMatrix(viewMatrix);
			}

			// Perspective Projection Matrix
			{
				float fieldOfView = 3.141592654f / 4.0f;
				float screenAspect = (float)m_pGraphicsDevice->GetResolutionWidth() / (float)m_pGraphicsDevice->GetResolutionHeight();

				auto projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);
				pBuffer->proj = XMMatrixTranspose(projMatrix);
				m_BufferFrameCPU.SetPerspectiveProjectionMatrix(projMatrix);
				auto view_proj = m_BufferFrameCPU.GetViewMatrix() * m_BufferFrameCPU.GetPerspectiveProjectionMatrix();
				m_BufferFrameCPU.SetViewProjectionMatrix(view_proj);
			}

			// Orthographic Projection Matrix
			{
				auto width = (float)m_pGraphicsDevice->GetResolutionWidth();
				auto height = (float)m_pGraphicsDevice->GetResolutionHeight();
				auto orthoProjMatrix = XMMatrixTranspose(XMMatrixOrthographicLH(width, height, 0.1f, 1000.0f));
				pBuffer->projOrthographic = orthoProjMatrix;
				m_BufferFrameCPU.SetOrthoProjectionMatrix(orthoProjMatrix);
			}
		}

		//immediateContext->Unmap(static_cast<ID3D11Resource*>(mConstantBufferMatrix.Get()), 0);
		m_pBufferFrame->Unmap();
	}

	void Renderer::DrawScene()
	{
		
	}

	void Renderer::DrawLegacy()
	{
		if (m_GameObjects[eObjectType::Opaque].empty())
			return;

		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(m_PipelineStateLegacy.pIL);
		immediateContext->IASetPrimitiveTopology(m_PipelineStateLegacy.primitiveTopology);
		immediateContext->VSSetShader(m_PipelineStateLegacy.pVS, NULL, 0);
		immediateContext->PSSetShader(m_PipelineStateLegacy.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &m_PipelineStateLegacy.pSS);
		immediateContext->OMSetDepthStencilState(m_PipelineStateLegacy.pDSS, 1);
		immediateContext->RSSetState(m_PipelineStateLegacy.pRSS);
		
		//m_pCamera->GetComponent<Camera>()->SetScreenSize(m_Viewport.Width, m_Viewport.Height);
		if (m_pCamera)
		{
			//auto viewport = m_pCamera->GetComponent<Camera>()->GetViewport();
			immediateContext->RSSetViewports(1, m_pCamera->GetComponent<Camera>()->GetViewportPtr());
		}
		else
			immediateContext->RSSetViewports(1, &m_Viewport);

		MeshRenderer* meshRenderer = nullptr;
		for (const auto& gameObject : m_GameObjects[eObjectType::Opaque])
		{
			if (Transform* transform = gameObject->GetTransform())
			{
				m_BufferObjectCPU.SetWorldMatrix(transform->GetMatrix());
				m_BufferObjectCPU.SetWorldViewProjectionMatrix(m_BufferObjectCPU.GetWorldMatrix() * m_BufferFrameCPU.GetViewProjectionMatrix());

				//==========================================================================//
				// Constant Buffer Test														//
				// 1. ���ĸ�ź�� �Լ��� ����� CPU �����͸� GPU�� map / unmap �ߴ�.			//
				// 2. DirectX�� ��İ� HLSL�� ��� ������ �ٸ��⶧���� ��ġ�� �־�� �Ѵ�.	//
				// ���� XMFLOAT4X4�� �����߱⿡ �� �� ����������.							//
				//==========================================================================//
				DirectX::XMMATRIX world = XMMatrixTranspose(m_BufferObjectCPU.GetWorldMatrix());
				DirectX::XMMATRIX wvp = XMMatrixTranspose(m_BufferObjectCPU.GetWorldViewProjectionMatrix());

				// �̰� ������Ʈ ���۴�. UpdateCB���� �������.
				BufferObject* pData = static_cast<BufferObject*>(m_pBufferObjectGPU->Map());
				DirectX::XMStoreFloat4x4(&pData->world, world);
				DirectX::XMStoreFloat4x4(&pData->wvp, wvp);
				assert(m_pBufferObjectGPU->Unmap());

				ID3D11Buffer* buffer = m_pBufferObjectGPU->GetBuffer();
				immediateContext->VSSetConstantBuffers(0, 1, &buffer);
			}

			meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (!meshRenderer)
				continue;

			meshRenderer->Render(immediateContext);

			/*
			ID3D11Buffer* vertexBuffer = meshRenderer->GetVertexBuffer();
			assert(vertexBuffer != nullptr);
			unsigned int stride = meshRenderer->GetVertexStride();
			unsigned int offset = 0;
			immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			ID3D11Buffer* indexBuffer = meshRenderer->GetIndexBuffer();
			assert(indexBuffer != nullptr);
			immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			immediateContext->DrawIndexed(meshRenderer->GetIndexCount(), 0, 0);
			*/
			//CORE_TRACE("Legacy Object - {0:d}, {1:d}", stride, meshRenderer->GetIndexCount());
		}
	}

	// �Լ��̸��� �ڵ����� �ٲ��...
	void Renderer::DrawText()
	{
		// GameObject���� TextMesh�� �̾Ƽ� ȣ���ϵ��� �ٲ�� �Ѵ�.

		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(m_PipelineStateFont.pIL);
		immediateContext->IASetPrimitiveTopology(m_PipelineStateFont.primitiveTopology);
		immediateContext->VSSetShader(m_PipelineStateFont.pVS, NULL, 0);
		immediateContext->PSSetShader(m_PipelineStateFont.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &m_PipelineStateFont.pSS);	// ��� ���� �����ʹ�...
		immediateContext->OMSetDepthStencilState(m_PipelineStateFont.pDSS, 1);
		immediateContext->RSSetState(m_PipelineStateFont.pRSS);

		ID3D11Buffer* buffer = m_pBufferFrame->GetBuffer();
		immediateContext->VSSetConstantBuffers(0, 1, &buffer);

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)m_pGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)m_pGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		immediateContext->RSSetViewports(1, &viewport);

		//auto pSRV = m_pTextMesh->GetAtlas();
		//immediateContext->PSSetShaderResources(0, 1, &pSRV);

		//auto pVB = m_pTextMesh->GetVertexBuffer();
		///unsigned int stride = m_pTextMesh->GetStride();
		//unsigned int offset = 0;
		//immediateContext->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		//immediateContext->IASetIndexBuffer(m_pTextMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		//immediateContext->DrawIndexed(m_pTextMesh->GetIndexCount(), 0, 0);
	}

	// =========================================================================//
	// Scene���� GameObjects�� ���ŵ� �� ���� ȣ��ȴ�.							//
	// ������ ������ Scene�� GameObject���� �̺�Ʈ�� �߻���Ű�� ���ε�			//
	// ���� �м��� �� �Ǿ���.													//
	//==========================================================================//
	void Renderer::ObjectClassify()
	{
		m_GameObjects.clear();
		// ī�޶� �ʱ�ȭ?

		// �ᱹ ������� �Դ�.
		auto& gameObjects = SceneManager::GetInstance().GetActiveScene()->GetAllGameObjects();
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive())
				continue;

			auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
			auto camera = gameObject->GetComponent<Camera>();
			auto light = gameObject->GetComponent<Light>();

			if (meshRenderer)
			{
				// Opaque�� Transparent�� ������ Material�� �̿��Ѵ�.
				m_GameObjects[eObjectType::Opaque].push_back(gameObject);
			}
			else if (camera)
			{
				m_GameObjects[eObjectType::Camera].push_back(gameObject);
				// ī�޶� ����... ���� ī�޶��ΰ�?
				m_pCamera = gameObject;
			}
			else if (light)
			{
				m_GameObjects[eObjectType::Light].push_back(gameObject);
			}
		}

		// Opaque�� Transparent�� �ٰŸ������� ���Ͽ� �������Ѵ�.

		// �ٵ� ������Ʈ �ֱ�� ��� �Ǵ°ɱ�?
		// �׸��� Frustum Culling�� ���⿡�� �����ϴ°� �����ΰ�?
	}

	void Renderer::SetGraphicsDevice(std::shared_ptr<GraphicsDevice> pDevice)
	{
		//m_pGraphicsDevice = pDevice;

		//assert(m_pGraphicsDevice);
	}	
}