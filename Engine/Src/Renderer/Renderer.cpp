#include "divepch.h"
#include "Renderer.h"
#include "../Scene/SceneManager.h"
#include "../Scene/Scene.h"
#include "../Scene/Component/MeshRenderer.h"
#include "../Scene/Component/dvMeshRenderer.h"
#include "../Scene/Component/Camera.h"
#include "../Scene/Component/Light.h"
#include "../Scene/GameObject.h"
#include "../Helper/Log.h"
#include "../Core/Event.h"
#include "../Scene/Component/Camera.h"
#include "../Core/DiveCore.h"

#include "Graphics/dvTexture2D.h"

namespace Dive
{
	Renderer::Renderer()
	{
		m_RenderTargetSize = DirectX::XMINT2(0, 0);
		// 이렇게 초기화하면 Sandbox에서 문제가 생긴다.
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

		// 리소스 제거
	}

	void Renderer::Initialize(HWND hWnd, bool fullScreen)
	{
		// 일단 구현
		m_pGraphicsDevice = new GraphicsDevice(hWnd, fullScreen);

		if (!m_pGraphicsDevice || !m_pGraphicsDevice->IsInitialized())
		{
			CORE_ERROR("Graphics Device가 생성되지 않아 초기화를 실행할 수 없습니다. 프로그램을 종료합니다.");
			PostQuitMessage(0);
		}

		// states
		createDepthStencilStates();
		createRasterizerStates();
		createSamplerStates();

		createConstantBuffers();
		createRenderTargets();		// 여러 RenderTexture들을 만든다.
		createShaders();

		//createFonts();

		createPipelineStates();	// 가장 마지막이어야 한다.

		// 그냥 테스트
		dvTexture2D dvTex(600, 400, DXGI_FORMAT_R8G8B8A8_UNORM, true);
		dvTex.Apply();
		
		CORE_TRACE("../Renderer 초기화에 성공하였습니다.");
	}

	//==============================================================================//
	// 이건 에바다. 현재 이 구현 때문에 Sandbox가 실행되지 않는다.					//
	// Camera는 자신이 그려질 RenderTarget의 크기를 스스로 찾아서 갱신해야 한다.	//
	// 그게 아니면 결국 이처럼 크기가 달라질 때 마다 Camera에게 알려줘야 한다.		//
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
			// 뭔가를 하는데...

			m_Viewport.Width = width;
			m_Viewport.Height = height;

			// 애초에 offset이 뭐냐...
		}
	}

	// 다시 한 번 말하지만 RenderTarget용 크기설정이다.
	void Renderer::SetResolution(unsigned int width, unsigned int height)
	{
		if (m_RenderTargetSize.x == width && m_RenderTargetSize.y == height)
			return;

		// 이외에도 크기가 맞는지 확인이 필요하다.

		m_RenderTargetSize.x = width;
		m_RenderTargetSize.y = height;

		createRenderTargets();

		CORE_TRACE("해상도 변경: {0:d}x{1:d}", width, height);
	}

	//======================================================================================//
	// 갱신만 한다. bind는 개별 path에서 한다.												//
	// 현재 wciked와 spartan의 구현이 꼬였다.												//
	// Main Camera를 통해 View, Projection을 이용한 행렬을 계산해							//
	// 매 프레임 갱신하는 Constant Buffer의 데이터를 저장(CPU)한다.							//
	// 현재 구버전 Constant Buffer에 하드 코딩된 View, Projection이 사용되어				//
	// 복잡하다.																			//
	//======================================================================================//
	void Renderer::UpdateCB()
	{
		auto immediateContext = m_pGraphicsDevice->GetImmediateContext();
		assert(immediateContext != nullptr);

		if (m_pBufferFrame == nullptr)
			return;

		MatrixBuffer* pBuffer = static_cast<MatrixBuffer*>(m_pBufferFrame->Map());

		// World Matrix
		// 이건 개별 GameObject의 Transform으로부터 가져온다.
		// 필요없는 것 같다. 예전 구현의 레거시다.
		pBuffer->world = XMMatrixTranspose(XMMatrixIdentity());

		// View와 Projection은 Camera로부터 가져온다.
		// => 카메라가 여러개일 수 있다.
		// => Scene 혹은 Camera의 static으로부터 활성화된 카메라를 모두 얻어와야 한다.
		// => 그렇게 되면 pass를 나누던가 하연튼 복잡해진다...
		if (m_pCamera)
		{
			auto pCamera = m_pCamera->GetComponent<Camera>();

			// view
			auto viewMatrix = pCamera->GetViewMatrix();
			pBuffer->view = XMMatrixTranspose(viewMatrix);

			// projection
			auto projMatrix = pCamera->GetProjectionMatrix();
			pBuffer->proj = XMMatrixTranspose(projMatrix);

			// 빈 값이라도 넣어야 하나??? 아니면 Projection은 하나만 전달토록 해야 하나...
			// 그것도 아니면 억지로라도 계산해서 두 개 다 넣어야 하나...
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

	// =========================================================================//
	// Scene에서 GameObjects가 갱신될 때 마다 호출된다.							//
	// 갱신의 시점은 Scene과 GameObject에서 이벤트를 발생시키는 것인데			//
	// 아직 분석이 덜 되었다.													//
	//==========================================================================//
	void Renderer::ObjectClassify()
	{
		m_GameObjects.clear();
		// 카메라도 초기화?

		// 결국 여기까지 왔다.
		auto& gameObjects = SceneManager::GetInstance().GetActiveScene()->GetAllGameObjects();
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive())
				continue;

			auto meshRenderer = gameObject->GetComponent<MeshRenderer>();
			auto pStaticMeshRenderer = gameObject->GetComponent<StaticMeshRenderer>();
			auto camera = gameObject->GetComponent<Camera>();
			auto light = gameObject->GetComponent<Light>();

			if (meshRenderer)
			{
				// Opaque와 Transparent의 구분은 Material을 이용한다.
				m_GameObjects[eObjectType::Opaque].push_back(gameObject);
			}
			else if (pStaticMeshRenderer)
			{
				// Opaque와 Transparent의 구분은 Material을 이용한다.
				m_GameObjects[eObjectType::Opaque].push_back(gameObject);
			}
			else if (camera)
			{
				m_GameObjects[eObjectType::Camera].push_back(gameObject);
				// 카메라 선택... 메인 카메라인가?
				// 즉, 이렇게 하면 안된다. 카메라는 여러개일 수 있고
				// 개념상으로 메인 카메라 역시 2개 이상일 수 있다.
				m_pCamera = gameObject;
			}
			else if (light)
			{
				m_GameObjects[eObjectType::Light].push_back(gameObject);
			}
		}

		// Opaque와 Transparent는 근거리순으로 비교하여 재정렬한다.

		// 근데 업데이트 주기는 어떻게 되는걸까?
		// 그리고 Frustum Culling을 여기에서 적용하는건 무리인가?
	}
}