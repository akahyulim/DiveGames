#include "Camera.h"


namespace dive
{
	Camera::Camera(GameObject* gameObject)
		: Component(typeid(Camera).hash_code(), gameObject)
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