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
		// 생성자.
		Transform(Context* pContext);
		// 소멸자.
		~Transform();

		// 변환 초기화. 부모가 존재할 경우 부모 좌표계를 기준으로 초기화.
		void Clear();

		// 월드 위치 리턴.
		DirectX::XMFLOAT3 GetPosition() const;
		// 로컬 위치 리턴.
		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		// 월드 위치 설정.
		void SetPosition(const DirectX::XMFLOAT3& pos);
		// 월드 위치 설정.
		void SetPosition(float xPos, float yPos, float zPos);
		// 로컬 위치 설정.
		void SetLocalPosition(const DirectX::XMFLOAT3& pos) { m_LocalPosition = pos; }
		// 로컬 위치 설정.
		void SetLocalPosition(float xPos, float yPos, float zPos);

		// 월드 회전 각도 리턴.
		DirectX::XMFLOAT3 GetRotation() const;
		// 월드 회전 쿼터니언 리턴.
		DirectX::XMFLOAT4 GetRotationQuaternion() const;
		// 로컬 회전 각도 리턴.
		DirectX::XMFLOAT3 GetLocalRotation() const;
		// 로컬 회전 쿼터니언 리턴.
		DirectX::XMFLOAT4 GetLocalRotationQuaternion() const { return m_LocalRotation; }
		// 전달받은 각도로 월드 회전.
		void SetRotation(const DirectX::XMFLOAT3& angle);
		// 전달받은 각도로 월드 회전.
		void SetRotation(float xAngle, float yAngle, float zAngle);
		// 전달받은 쿼터니언으로 월드 회전.
		void SetRotation(const DirectX::XMFLOAT4& quaternion);
		// 전달받은 각도로 로컬 회전.
		void SetLocalRotation(const DirectX::XMFLOAT3& angle);
		// 전달받은 각도로 로컬 회전.
		void SetLocalRotation(float xAngle, float yAngle, float zAngle);
		// 전달받은 쿼터니언으로 로컬 회전.
		void SetLocalRotation(const DirectX::XMFLOAT4& quaternion) { m_LocalRotation = quaternion; }

		// 월드 스케일 리턴.
		DirectX::XMFLOAT3 GetScale() const;
		// 로컬 스케일 리턴.
		DirectX::XMFLOAT3 GetLocalScale() const { return m_LocalScale; }
		// 월드 스케일 설정.
		void SetScale(const DirectX::XMFLOAT3& scale);
		// 월드 스케일 설정.
		void SetScale(float xScale, float yScale, float zScale);
		// 로컬 스케일 설정.
		void SetLocalScale(const DirectX::XMFLOAT3& scale) { m_LocalScale = scale; }
		// 로컬 스케일 설정.
		void SetLocalScale(float xScale, float yScale, float zScale);

		// 변환 행렬 리턴.
		DirectX::XMMATRIX GetMatrix();
		// 로컬 변환 행렬 리턴.
		DirectX::XMMATRIX GetLocalMatrix() const;
		// 로컬 변환 행렬 설정.
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

		// 전방 벡터.
		DirectX::XMFLOAT3 GetForward() const;
		// 상향 벡터.
		DirectX::XMFLOAT3 GetUp() const;
		// 우향 벡터.
		DirectX::XMFLOAT3 GetRight() const;

		// 부모 트랜스폼 존재여부 리턴.
		bool HasParent() const { return m_pParent; }
		// 부모 트랜스폼 포인터 리턴.
		Transform* GetParent() const { return m_pParent; }
		// 부모 트랜스폼 설정.
		void SetParent(Transform* pParent);
		// 루트 트랜스폼 리턴.
		Transform* GetRoot();
		// 자식 존재여부 리턴.
		bool HasChild() const { return !m_Children.empty(); }
 		// 자식 수 리턴.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(m_Children.size()); }
		// 전달된 인덱스에 맞는 자식 포인터 리턴.
		Transform* GetChild(unsigned int index) const;
		// 현재 트랜스폼이 전달받은 트랜스폼의 자식 여부를 리턴.
		bool IsChildOf(Transform* pParent) const;
		// 모든 자식을 독립.
		void DetachChildren();
		// 자식 벡터 리턴.
		std::vector<Transform*> GetChildren() const;

	private:
	private:
		// 로컬 위치 벡터.
		DirectX::XMFLOAT3 m_LocalPosition;
		// 로컬 회전 쿼터니언.
		DirectX::XMFLOAT4 m_LocalRotation;
		// 로컬 스케일 벡터.
		DirectX::XMFLOAT3 m_LocalScale;

		// 월드 변환 행렬.
		DirectX::XMFLOAT4X4 m_Matrix;

		// 부모 트랜스폼 포인터.
		Transform* m_pParent;
		// 자식 트랜스폼.
		std::vector<Transform*> m_Children;
	};
}
