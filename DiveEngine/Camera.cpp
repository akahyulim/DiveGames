#include "Camera.h"


namespace dive
{
	Camera::Camera(GameObject* pGameObject)
		: Component(typeid(Camera).hash_code(), pGameObject)
	{
	}
	
	Camera::~Camera()
	{
	}

	void Camera::Update(float deltaTime)
	{
		// ���� ��ġ�� ���� ��ġ�� ���Ͽ� 
		// dirty check�� �Ѵ�.
	}

	DirectX::XMMATRIX Camera::computeViewMatrix() const
	{
		// ��ġ
		// �ٶ󺸴� ���� = ȸ�� ����� * ���� ����
		// ���� ���� = ȸ�� ����� * ���� ����

		// �ٶ󺸴� ���� += ��ġ

		// �� ������ ���� �̿��� LookAtLH ���

		return DirectX::XMMATRIX();
	}
}