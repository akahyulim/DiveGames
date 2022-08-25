#pragma once
#include "Component.h"

namespace Dive
{
	class Context;

	enum class eSpace
	{
		World,
		Self
	};

	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component)

	public:
		// ������.
		Transform(Context* pContext);
		// �Ҹ���.
		~Transform();

		// ��ȯ �ʱ�ȭ. �θ� ������ ��� �θ� ��ǥ�踦 �������� �ʱ�ȭ.
		void Clear();

		// ���� ��ġ ����.
		DirectX::XMFLOAT3 GetPosition() const;
		// ���� ��ġ ����.
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		// ���� ��ġ ����.
		void SetPosition(const DirectX::XMFLOAT3& pos);
		// ���� ��ġ ����.
		void SetPosition(float xPos, float yPos, float zPos);
		// ���� ��ġ ����.
		void SetLocalPosition(const DirectX::XMFLOAT3& pos) { m_LocalPosition = pos; }
		// ���� ��ġ ����.
		void SetLocalPosition(float xPos, float yPos, float zPos);

		// ���� ȸ�� ���� ����.
		DirectX::XMFLOAT3 GetRotation() const;
		// ���� ȸ�� ���ʹϾ� ����.
		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		// ���� ȸ�� ���� ����.
		DirectX::XMFLOAT3 GetLocalRotation() const;
		// ���� ȸ�� ���ʹϾ� ����.
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		// ���޹��� ������ ���� ȸ��.
		void SetRotation(const DirectX::XMFLOAT3& angle);
		// ���޹��� ������ ���� ȸ��.
		void SetRotation(float xAngle, float yAngle, float zAngle);
		// ���޹��� ���ʹϾ����� ���� ȸ��.
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		// ���޹��� ������ ���� ȸ��.
		void SetLocalRotation(const DirectX::XMFLOAT3& angle);
		// ���޹��� ������ ���� ȸ��.
		void SetLocalRotation(float xAngle, float yAngle, float zAngle);
		// ���޹��� ���ʹϾ����� ���� ȸ��.
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion) { m_LocalRotation = quaternion; }

		// ���� ������ ����.
		DirectX::XMFLOAT3 GetScale() const;
		// ���� ������ ����.
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		// ���� ������ ����.
		void SetScale(const DirectX::XMFLOAT3& scale);
		// ���� ������ ����.
		void SetScale(float xScale, float yScale, float zScale);
		// ���� ������ ����.
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_LocalScale = scale; }
		// ���� ������ ����.
		void SetLocalScale(float xScale, float yScale, float zScale);

		// ��ȯ ��� ����.
		DirectX::XMMATRIX GetMatrix();
		// ���� ��ȯ ��� ����.
		DirectX::XMMATRIX GetLocalMatrix() const;
		// ���� ��ȯ ��� ����.
		void SetLocalMatrix(const DirectX::XMFLOAT4X4& mat);

		void Translate(DirectX::XMFLOAT3 translation, eSpace relativeTo = eSpace::Self);
		void Translate(float x, float y, float z, eSpace relativeTo = eSpace::Self);
		void Translate(DirectX::XMFLOAT3 translation, Transform relativeTo);
		void Translate(float x, float y, float z, Transform relativeTo);

		void Rotate(DirectX::XMFLOAT3 eulerAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(float xAngle, float yAngle, float zAngle, eSpace relativeTo = eSpace::Self);
		void Rotate(DirectX::XMFLOAT3 axis, float angle, eSpace relativeTo = eSpace::Self);

		void RotateAround(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 axis, float angle);

		void LookAt(Transform target, DirectX::XMFLOAT3 worldUp = {0.0f, 1.0f, 0.0f});
		void LookAt(DirectX::XMFLOAT3 worldPosition, DirectX::XMFLOAT3 worldUp = { 0.0f, 1.0f, 0.0f });

		// ���� ����.
		DirectX::XMFLOAT3 GetForward() const;
		// ���� ����.
		DirectX::XMFLOAT3 GetUp() const;
		// ���� ����.
		DirectX::XMFLOAT3 GetRight() const;

		// �θ� Ʈ������ ���翩�� ����.
		bool HasParent() const { return m_pParent; }
		// �θ� Ʈ������ ������ ����.
		Transform* GetParent() const { return m_pParent; }
		// �θ� Ʈ������ ����.
		void SetParent(Transform* pParent);
		// ��Ʈ Ʈ������ ����.
		Transform* GetRoot();
		// �ڽ� ���翩�� ����.
		bool HasChild() const { return !m_Children.empty(); }
 		// �ڽ� �� ����.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		// ���޵� �ε����� �´� �ڽ� ������ ����.
		Transform* GetChild(unsigned int index) const;
		// ���� Ʈ�������� ���޹��� Ʈ�������� �ڽ� ���θ� ����.
		bool IsChildOf(Transform* pParent) const;
		// ��� �ڽ��� ����.
		void DetachChildren();
		// �ڽ� ���� ����.
		std::vector<Transform*> GetChildren() const;

	private:
	private:
		// ���� ��ġ ����.
		DirectX::XMFLOAT3 m_LocalPosition;
		// ���� ȸ�� ���ʹϾ�.
		DirectX::XMFLOAT4 m_LocalRotation;
		// ���� ������ ����.
		DirectX::XMFLOAT3 m_LocalScale;

		// ���� ��ȯ ���.
		DirectX::XMFLOAT4X4 m_Matrix;

		// �θ� Ʈ������ ������.
		Transform* m_pParent;
		// �ڽ� Ʈ������.
		std::vector<Transform*> m_Children;
	};
}
