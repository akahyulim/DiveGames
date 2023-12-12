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

// �ᱹ Ư�� ���� �������� ȸ�� �� ���¿��� �ٸ� ���� ȸ���ϴ� ����� �̻��������δ�.
// ������ �̰� �߸��Ȱ��� �ƴϸ� �߸� �����ϰ� �޾Ƶ��̴� ������ �а����� �ʴ´ٴ� ���̴�.
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
		// ���� Ű �Է°� ���콺 �̵��� ���ÿ� �۵����� �ʴ´�.
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