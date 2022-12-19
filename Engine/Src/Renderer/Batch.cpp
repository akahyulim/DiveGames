#include "divepch.h"
#include "Batch.h"
#include "Graphics/Graphics.h"
#include "Renderer/View.h"
#include "Renderer/Technique.h"
#include "IO/Log.h"

namespace Dive
{
	// InstancingBatch에도 사용되므로 public이어야 한다.
	void Batch::Prepare(View* pView)
	{
		if (!m_pVertexShaderVariation || !m_pPixelShaderVariation)
			return;

		pView->GetGraphics()->SetShaders(m_pVertexShaderVariation, m_pPixelShaderVariation);

		// Pass를 이용해
		// graphics에 blend, cull, fill mode등을 전달한다.

		// shaderParameter들을 graphics에 보낸다.
		{
			//pView->GetGraphics()->SetShaderParameter("cModel", *m_pWorldTransform);
			auto worldTransform = DirectX::XMLoadFloat4x4(m_pWorldTransform);
			worldTransform = DirectX::XMMatrixTranspose(worldTransform);
			pView->GetGraphics()->SetShaderParameter("cModel", worldTransform);
		}

		// material의 텍스쳐를 graphics를 통해 set한다.
		if (m_pMaterial)
		{

		}
	}

	void Batch::Draw(View* pView)
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

			//pView->GetGraphics()->SetShaderParameter("cModel", worldMatrix);
			pView->GetGraphics()->SetShaderParameter("viewMatrix", viewMatrix);
			pView->GetGraphics()->SetShaderParameter("projectionMatrix", projMatrix);
		}

		m_pMesh->Draw(pView->GetGraphics());
	}
}