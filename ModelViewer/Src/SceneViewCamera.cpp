#include "SceneViewCamera.h"

using namespace Dive;

SceneViewCamera::SceneViewCamera(Dive::GameObject* pGameObject)
	: Dive::Camera(pGameObject)
	, m_bBoost(false)
{
}

SceneViewCamera::~SceneViewCamera()
{
}

// 결국 특정 축을 기준으로 회전 된 상태에서 다른 축을 회전하니 결과가 이상해져보인다.
// 문제가 이게 잘못된건지 아니면 잘못 이해하고 받아들이는 것인지 분간되지 않는다는 것이다.
void SceneViewCamera::Update()
{
	if (!Input::GetMouseIsInValidRect())
		return;

	float deltaTime = static_cast<float>(Timer::GetDeltaTimeSec());

	auto pTransform = m_pGameObject->GetTransform();

	static float boost = 1.0f;
	if (Input::KeyPress(DIK_LSHIFT))
		boost = 5.0f;
	else
		boost = 1.0f;

	if (Input::KeyPress(DIK_UP))
	{
		pTransform->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost);
	}
	if (Input::KeyPress(DIK_DOWN))
	{
		pTransform->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost);
	}
	if (Input::KeyPress(DIK_LEFT))
	{
		pTransform->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
	}
	if (Input::KeyPress(DIK_RIGHT))
	{
		pTransform->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
	}

	if (Input::MouseButtonPress(1))
	{
		// 현재 키 입력과 마우스 이동이 동시에 작동하지 않는다.
		const DirectX::XMFLOAT2& delta = Input::GetMouseMoveDelta();
		pTransform->Rotate(delta.y * 0.1f, delta.x * 0.1f, 0.0f);

		if (Input::KeyPress(DIK_W))
		{
			pTransform->Translate(0.0f, 0.0f, m_MoveSpeed * deltaTime * boost);
		}
		if (Input::KeyPress(DIK_S))
		{
			pTransform->Translate(0.0f, 0.0f, -m_MoveSpeed * deltaTime * boost);
		}
		if (Input::KeyPress(DIK_A))
		{
			pTransform->Translate(-m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
		}
		if (Input::KeyPress(DIK_D))
		{
			pTransform->Translate(m_MoveSpeed * deltaTime * boost, 0.0f, 0.0f);
		}

		if (Input::KeyPress(DIK_Q))
		{
			pTransform->Translate(0.0f, m_MoveSpeed * deltaTime * boost, 0.0f);
		}
		if (Input::KeyPress(DIK_E))
		{
			pTransform->Translate(0.0f, -m_MoveSpeed * deltaTime * boost, 0.0f);
		}
	}
}