#include "Camera.h"


namespace dive
{
	Camera::Camera(GameObject* owner)
		: Component(typeid(Camera).hash_code(), owner)
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