#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"
#include "Log.h"

namespace dive
{
	Transform::Transform(GameObject* pGameObject)
		: Component(typeid(Transform).hash_code(), pGameObject, this)
	{
		m_LocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		DirectX::XMStoreFloat4x4(&m_Matrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_LocalMatrix, DirectX::XMMatrixIdentity());

		m_LookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 이건 추후 바꿔야 한다.
	}

	//======================================================//
	// 이게 호출이 안되네...								//
	// GameObject 혹은 Scene 제거가 제대로 안된다는 소리다.	//
	// 그리고보니 SceneManager때문인 것 같다.				//
	// SceneManager에서 Clear 등을 만들면 될 것 같다.		//
	//======================================================//
	Transform::~Transform()
	{
		CORE_TRACE("Transform 소멸자 호출");
		//CORE_TRACE("name : {0:s}, parent name : {1:s}", GetGameObject()->GetName(), HasParent() ? GetParent()->GetGameObject()->GetName() : "nope");
			//world pos({1:f}, {2:f}, {3:f}), local pos({4:f}, {5:f}, {6:f})",
	}

	void Transform::Update(float deltaTime)
	{
		
	}

	void Transform::Serialize(FileStream* pFileStream)
	{
		// local rot
		pFileStream->Write(m_LocalPosition);
		pFileStream->Write(m_LocalScale);
		// lookAt
		pFileStream->Write(m_pParent ? m_pParent->GetGameObject()->GetInstanceID() : 0);
	}

	void Transform::Deserialize(FileStream* pFileStream)
	{
		// local rot
		pFileStream->Read(&m_LocalPosition);
		pFileStream->Read(&m_LocalScale);
		// lookAt
		unsigned int parentId = 0;
		pFileStream->Read(&parentId);

		// update pTransform
	}

	// 변환 행렬은 구현을 완료하였다.
	void Transform::UpdateTransform()
	{
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(getLocalPositionByVector());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixIdentity();//DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector());
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixScalingFromVector(getLocalScaleByVector());

		auto localMatrix = matLocalScl * matLocalRot * matLocalPos;
		DirectX::XMStoreFloat4x4(&m_LocalMatrix, localMatrix);

		if (HasParent())
		{
			DirectX::XMStoreFloat4x4(&m_Matrix, (localMatrix * m_pParent->GetMatrix()));
		}
		else
		{
			m_Matrix = m_LocalMatrix;
		}

		for (auto child : m_Children)
		{
			child->UpdateTransform();
		}
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return DirectX::XMFLOAT3(m_Matrix._41, m_Matrix._42, m_Matrix._43);
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		setPositionByVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		setLocalPositionByVector(DirectX::XMLoadFloat3(&position));
	}
	
	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		if (HasParent())
		{
			auto scale = DirectX::XMVector3Transform(getLocalScaleByVector(), m_pParent->GetMatrix());
			DirectX::XMFLOAT3 result;
			DirectX::XMStoreFloat3(&result, scale);

			return result;
		}

		return m_LocalScale;
	}
	
	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		setScaleByVector(DirectX::XMLoadFloat3(&scale));
	}

	// 이건 다시 setLocalScaleByVector로 바꾸자
	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		// 흐음... 0.0이어도 안되는데

		if ((m_LocalScale.x == scale.x) && (m_LocalScale.y == scale.y) && (m_LocalScale.z == scale.z))
			return;

		m_LocalScale = scale;

		UpdateTransform();
	}

	void Transform::SetLookAt(float x, float y, float z)
	{
		m_LookAt = DirectX::XMFLOAT3(x, y, z);
	}

	DirectX::XMVECTOR Transform::getPositionByVector() const
	{
		return DirectX::XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	void Transform::setPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getPositionByVector()))
			return;

		setLocalPositionByVector(HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix()))
			: position);
	}

	DirectX::XMVECTOR Transform::getLocalPositionByVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	// scale처럼 바꿀건지 선택해야 한다.
	void Transform::setLocalPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getLocalPositionByVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		UpdateTransform();
	}

	// 일단 World Scale 계산은 틀린 것 같다.
	DirectX::XMVECTOR Transform::getScaleByVector() const
	{
		if (HasParent())
		{
			auto scale = DirectX::XMVector3Transform(getLocalScaleByVector(), m_pParent->GetMatrix());
			return scale;
		}

		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	void Transform::setScaleByVector(const DirectX::FXMVECTOR& scale)
	{
		// 비교는 나중에

		if (HasParent())
		{
			// 부모가 있을 때
			// 일단 여기에서 역 곱하기
			setLocalScaleByVector(DirectX::XMVector3Transform(scale, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix())));
		}
		else
		{
			// 부모가 없을 때
			setLocalScaleByVector(scale);
		}
	}

	// 로컬은 그냥 가져와도 되나?
	DirectX::XMVECTOR Transform::getLocalScaleByVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	void Transform::setLocalScaleByVector(const DirectX::FXMVECTOR& scale)
	{
		DirectX::XMFLOAT3 destination;
		DirectX::XMStoreFloat3(&destination, scale);

		SetLocalScale(destination);
	}
}