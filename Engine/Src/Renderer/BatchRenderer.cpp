#include "divepch.h"
#include "BatchRenderer.h"
#include "Graphics/Graphics.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
#include "IO/Log.h"

namespace Dive
{
	// shader와 같이 개별 mesh에 적용되어야 할 리소스를 gpu로 보낸다.
	void BatchRenderer::Prepare(View* pView)
	{
		// 일단 ShaderVariation에 vs, ps 둘 중 하나라도 없으면 리턴이다.

		// 이건 임시다. ShaderVariation를 가지고 있어야 한다.
		// 현재 Material, Technique, Pass의 과정을 거쳐 Shader의 이름과 Defines에 접근이 가능하다.
		// => ShaderVariation 생성 및 설정까지 끝마친 후 코드를 다듬자.
		pView->GetGraphics()->SetDefaultShader();

		// Pass를 이용해
		// graphics에 blend, cull, fill mode등을 전달한다.

		// shaderParameter들을 graphics에 보낸다.

		// material의 텍스쳐를 graphics를 통해 set한다.
		if (m_pMaterial)
		{

		}
	}

	void BatchRenderer::Draw(View* pView)
	{
		if (!m_pMesh)
			return;

		Prepare(pView);

		// test parameter
		{
			using namespace DirectX;

			// world
			auto worldMatrix = DirectX::XMMatrixIdentity();

			// view
			XMFLOAT3 up, position, lookAt;
			XMVECTOR upVector, positionVector, lookAtVector;
			float yaw, pitch, roll;
			XMMATRIX rotationMatrix;


			// Setup the vector that points upwards.
			up.x = 0.0f;
			up.y = 1.0f;
			up.z = 0.0f;

			// Load it into a XMVECTOR structure.
			upVector = XMLoadFloat3(&up);

			// Setup the position of the camera in the world.
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
			pitch = 0 * 0.0174532925f;
			yaw = 0 * 0.0174532925f;
			roll = 0 * 0.0174532925f;

			// Create the rotation matrix from the yaw, pitch, and roll values.
			rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

			// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
			lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
			upVector = XMVector3TransformCoord(upVector, rotationMatrix);

			// Translate the rotated camera position to the location of the viewer.
			lookAtVector = XMVectorAdd(positionVector, lookAtVector);

			// Finally create the view matrix from the three updated vectors.
			auto viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

			// proj
			float fieldOfView = 3.141592654f / 4.0f;
			float screenAspect = static_cast<float>(pView->GetGraphics()->GetWidth()) / static_cast<float>(pView->GetGraphics()->GetHeight());
			auto projMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);

			// constant buffer를 만들어 vs에 전달해야 한다.
			worldMatrix = XMMatrixTranspose(worldMatrix);
			viewMatrix = XMMatrixTranspose(viewMatrix);
			projMatrix = XMMatrixTranspose(projMatrix);

			pView->GetGraphics()->SetShaderParameter("worldMatrix", worldMatrix);
			pView->GetGraphics()->SetShaderParameter("viewMatrix", viewMatrix);
			pView->GetGraphics()->SetShaderParameter("projectionMatrix", projMatrix);
		}

		m_pMesh->Draw(pView->GetGraphics());
	}
}