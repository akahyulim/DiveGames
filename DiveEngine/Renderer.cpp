#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Log.h"
#include "TextMesh.h"
#include <assert.h>

namespace dive
{
	Renderer::Renderer()
	{
		mRenderTargetSize = DirectX::XMINT2(0, 0);
		// �̷��� �ʱ�ȭ�ϸ� Sandbox���� ������ �����.
		mViewPort.Width = static_cast<float>(mRenderTargetSize.x);
		mViewPort.Height = static_cast<float>(mRenderTargetSize.y);
		//mViewPort.Width = (float)mGraphicsDevice->GetResolutionWidth();
		//mViewPort.Height = (float)mGraphicsDevice->GetResolutionHeight();
		mViewPort.MinDepth = 0.0f;
		mViewPort.MaxDepth = 1.0f;
		mViewPort.TopLeftX = 0.0f;
		mViewPort.TopLeftY = 0.0f;
	}

	Renderer::~Renderer()
	{
		// ���ҽ� ����
	}

	void Renderer::Initialize()
	{
		if (!mGraphicsDevice || !mGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("Graphics Device�� �������� �ʾ� �ʱ�ȭ�� ������ �� �����ϴ�. ���α׷��� �����մϴ�.");
			PostQuitMessage(0);
		}

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

	void Renderer::SetViewport(float width, float height, float offsetX, float offsetY)
	{
		if (mViewPort.Width != width || mViewPort.Height != height)
		{
			// ������ �ϴµ�...

			mViewPort.Width = width;
			mViewPort.Height = height;

			// ���ʿ� offset�� ����...
		}
	}

	// �ٽ� �� �� �������� RenderTarget�� ũ�⼳���̴�.
	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		if (mRenderTargetSize.x == width && mRenderTargetSize.y == height)
			return;

		// �̿ܿ��� ũ�Ⱑ �´��� Ȯ���� �ʿ��ϴ�.

		mRenderTargetSize.x = width;
		mRenderTargetSize.y = height;

		createRenderTargets();

		CORE_TRACE("�ػ� ����: {0:d}x{1:d}", width, height);
	}

	// ���Ÿ� �Ѵ�. bind�� ���� path���� �Ѵ�.
	void Renderer::UpdateCB()
	{
		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		// CB Update
		if (mConstantBufferMatrix == nullptr)
			return;

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (FAILED(immediateContext->Map(mConstantBufferMatrix.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			CORE_ERROR("Constant Buffer Mapping�� �����Ͽ����ϴ�.");
			return;
		}

		MatrixBuffer* pBuffer = static_cast<MatrixBuffer*>(mappedResource.pData);
		
		// World Matrix
		pBuffer->world = XMMatrixTranspose(XMMatrixIdentity());

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
			pBuffer->view = XMMatrixTranspose(XMMatrixLookAtLH(positionVector, lookAtVector, upVector));
		}

		// Perspective Projection Matrix
		{
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = (float)mGraphicsDevice->GetResolutionWidth() / (float)mGraphicsDevice->GetResolutionHeight();
			pBuffer->proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f));
		}

		// Orthographic Projection Matrix
		{
			auto width = (float)mGraphicsDevice->GetResolutionWidth();
			auto height = (float)mGraphicsDevice->GetResolutionHeight();
			pBuffer->projOrthographic = XMMatrixTranspose(XMMatrixOrthographicLH(width, height, 0.1f, 1000.0f));
		}

		immediateContext->Unmap(static_cast<ID3D11Resource*>(mConstantBufferMatrix.Get()), 0);
	}

	void Renderer::DrawScene()
	{
		
	}

	// �̰� �����Ϸ��� IL�� �¾ƾ� �Ѵ�.
	void Renderer::DrawColor()
	{
		// �̰� �ӽô�. Visibility ���� ���� ���� �Ѵ�.
		Mesh* mesh = Scene::GetGlobalScene().GetMesh();
		if (!mesh)
			return;

		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(mPipelineStateColor.pIL);
		immediateContext->IASetPrimitiveTopology(mPipelineStateColor.primitiveTopology);
		immediateContext->VSSetShader(mPipelineStateColor.pVS, NULL, 0);
		immediateContext->PSSetShader(mPipelineStateColor.pPS, NULL, 0);
		immediateContext->OMSetDepthStencilState(mPipelineStateColor.pDSS, 1);
		immediateContext->RSSetState(mPipelineStateColor.pRSS);

		immediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());
		immediateContext->RSSetViewports(1, &mViewPort);

		ID3D11Buffer* vbs[] =
		{
			mesh->m_pVBPosition.Get(),
			mesh->m_pVBColor.Get()
		};

		UINT strides[] =
		{
			sizeof(XMFLOAT3),
			sizeof(XMFLOAT4)
		};

		unsigned int offsets[] =
		{
			0,
			0
		};

		immediateContext->IASetVertexBuffers(0, arraysize(vbs), vbs, strides, offsets);
		immediateContext->IASetIndexBuffer(mesh->m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		immediateContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	void Renderer::DrawTexturing()
	{
		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(mPipelineStateTexturing.pIL);
		immediateContext->IASetPrimitiveTopology(mPipelineStateTexturing.primitiveTopology);
		immediateContext->VSSetShader(mPipelineStateTexturing.pVS, NULL, 0);
		immediateContext->PSSetShader(mPipelineStateTexturing.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &mPipelineStateTexturing.pSS);	// ��� ���� �����ʹ�...
		immediateContext->OMSetDepthStencilState(mPipelineStateTexturing.pDSS, 1);
		immediateContext->RSSetState(mPipelineStateTexturing.pRSS);

		immediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());


		// �ӽ�
		// ���� ������ �ұ�?
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)mGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)mGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		immediateContext->RSSetViewports(1, &viewport);

		// �̰� �ӽô�. Visibility ���� ���� ���� �Ѵ�.
		Mesh* mesh = Scene::GetGlobalScene().GetMesh();

		ID3D11Buffer* vbs[] =
		{
			mesh->m_pVBPosition.Get(),
			mesh->m_pVBUv.Get()
		};

		UINT strides[] =
		{
			sizeof(XMFLOAT3),
			sizeof(XMFLOAT2)
		};

		unsigned int offsets[] =
		{
			0,
			0
		};

		// ��ġ�� ����� ������ ���� �ʴ´�.
		auto pSRV = mTexture->GetShaderResourceView();
		//auto pSRV = mCpuTexture->GetShaderResourceView();
		//auto pSRV = mDvFont->GetAtlas()->GetShaderResourceView();
		immediateContext->PSSetShaderResources(0, 1, &pSRV);

		immediateContext->IASetVertexBuffers(0, arraysize(vbs), vbs, strides, offsets);
		immediateContext->IASetIndexBuffer(mesh->m_pIB.Get(), DXGI_FORMAT_R32_UINT, 0);

		immediateContext->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	void Renderer::DrawLegacy()
	{
		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(mPipelineStateLegacy.pIL);
		immediateContext->IASetPrimitiveTopology(mPipelineStateLegacy.primitiveTopology);
		immediateContext->VSSetShader(mPipelineStateLegacy.pVS, NULL, 0);
		immediateContext->PSSetShader(mPipelineStateLegacy.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &mPipelineStateLegacy.pSS);
		immediateContext->OMSetDepthStencilState(mPipelineStateLegacy.pDSS, 1);
		immediateContext->RSSetState(mPipelineStateLegacy.pRSS);

		immediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)mGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)mGraphicsDevice->GetResolutionHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		immediateContext->RSSetViewports(1, &viewport);

		// ���� ���͸� �����ؼ� �ް� �ִ�. ������ �ξ� ������... 
		auto gameObjects = Scene::GetGlobalScene().GetAllGameObjects();
		MeshRenderer* meshRenderer = nullptr;
		for (const auto& gameObject : gameObjects)
		{
			meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer)
			{
				ID3D11Buffer* vertexBuffer = meshRenderer->GetVertexBuffer();
				assert(vertexBuffer != nullptr);
				unsigned int stride = meshRenderer->GetVertexStride();
				unsigned int offset = 0;
				immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
				ID3D11Buffer* indexBuffer = meshRenderer->GetIndexBuffer();
				assert(indexBuffer != nullptr);
				immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

				immediateContext->DrawIndexed(meshRenderer->GetIndexCount(), 0, 0);

				CORE_TRACE("Legacy Object - {0:d}, {1:d}", stride, meshRenderer->GetIndexCount());
			}
		}
	}

	// �Լ��̸��� �ڵ����� �ٲ��...
	void Renderer::DrawText()
	{
		// GameObject���� TextMesh�� �̾Ƽ� ȣ���ϵ��� �ٲ�� �Ѵ�.

		auto immediateContext = mGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		immediateContext->IASetInputLayout(mPipelineStateFont.pIL);
		immediateContext->IASetPrimitiveTopology(mPipelineStateFont.primitiveTopology);
		immediateContext->VSSetShader(mPipelineStateFont.pVS, NULL, 0);
		immediateContext->PSSetShader(mPipelineStateFont.pPS, NULL, 0);
		immediateContext->PSSetSamplers(0, 1, &mPipelineStateFont.pSS);	// ��� ���� �����ʹ�...
		immediateContext->OMSetDepthStencilState(mPipelineStateFont.pDSS, 1);
		immediateContext->RSSetState(mPipelineStateFont.pRSS);

		immediateContext->VSSetConstantBuffers(0, 1, mConstantBufferMatrix.GetAddressOf());

		D3D11_VIEWPORT viewport;
		viewport.Width = (float)mGraphicsDevice->GetResolutionWidth();
		viewport.Height = (float)mGraphicsDevice->GetResolutionHeight();
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

	// ���ĸ�ź�� �̺�Ʈ callback �Լ���.
	// �׷��� ���� �� �̺�Ʈ �ý��ۿ��� Variant�� �Ű������� ���� �� ����.
	void Renderer::ObjectClassify()
	{
		// ��� GameObject�� ��ȸ�ϸ鼭 Component ���η� ������ �����Ѵ�.
		// Opaque�� Transparent�� ������ Material�� �̿��Ѵ�.

		// Opaque�� Transparent�� �ٰŸ������� ���Ͽ� �������Ѵ�.

		// �ٵ� ������Ʈ �ֱ�� ��� �Ǵ°ɱ�?
		// �׸��� Frustum Culling�� ���⿡�� �����ϴ°� �����ΰ�?
	}

	void Renderer::SetGraphicsDevice(std::shared_ptr<GraphicsDevice> device)
	{
		mGraphicsDevice = device;

		assert(mGraphicsDevice);
	}	
}