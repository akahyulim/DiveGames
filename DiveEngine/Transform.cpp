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
		DirectX::XMStoreFloat4(&m_LocalRotation, DirectX::XMQuaternionIdentity());
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
		//test

		translate(DirectX::XMVectorSet(0.0f, 0.1f, 0.0f, 1.0f) * deltaTime, eSpace::World);
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
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(getLocalPosition());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixIdentity();//DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector());
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixScalingFromVector(getLocalScale());

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
		setPosition(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		setLocalPosition(DirectX::XMLoadFloat3(&position));
	}

	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		return DirectX::XMFLOAT4();
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& rotation)
	{
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& rotation)
	{
	}

	void Transform::SetRotation(float xAngle, float yAngle, float zAngle)
	{
		// 일단 라디안으로 바꾼 후 어떻게 한다.
	}
	
	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		if (HasParent())
		{
			auto scale = DirectX::XMVector3Transform(getLocalScale(), m_pParent->GetMatrix());
			DirectX::XMFLOAT3 result;
			DirectX::XMStoreFloat3(&result, scale);

			return result;
		}

		return m_LocalScale;
	}
	
	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		setScale(DirectX::XMLoadFloat3(&scale));
	}

	// 이건 다시 setLocalScale로 바꾸자
	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		// 흐음... 0.0이어도 안되는데

		if ((m_LocalScale.x == scale.x) && (m_LocalScale.y == scale.y) && (m_LocalScale.z == scale.z))
			return;

		m_LocalScale = scale;

		UpdateTransform();
	}

	// 특성상 deltaTime을 곱하는 경우가 많은데 XMFLOAT3는 그게 안된다.
	void Transform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		translate(DirectX::XMLoadFloat3(&translation), relativeTo);
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		translate(DirectX::XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	void Transform::Rotate(const DirectX::XMFLOAT3& enulerAngles)
	{
	}

	void Transform::Rotate(float xAngle, float yAngle, float zAngle)
	{
	}

	void Transform::SetLookAt(float x, float y, float z)
	{
		m_LookAt = DirectX::XMFLOAT3(x, y, z);
	}

	DirectX::XMVECTOR Transform::getPosition() const
	{
		return DirectX::XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	void Transform::setPosition(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getPosition()))
			return;

		setLocalPosition(HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix()))
			: position);
	}

	DirectX::XMVECTOR Transform::getLocalPosition() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	// 이게 낫다.
	void Transform::setLocalPosition(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getLocalPosition()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		UpdateTransform();
	}

	DirectX::XMVECTOR Transform::getRotation() const
	{
		// 이게 맞나...
		XMFLOAT3 scale;
		XMStoreFloat3(&scale, getScale());

		if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)
			return XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMFLOAT4X4 mat;
		XMStoreFloat4x4(&mat, GetMatrix());

		// 결국 변환행렬에서 각 요소의 scale을 나눈 후 결과를 다시 쿼터니언으로 변환하는 거다.
		// 문제는 변환행렬에서 scale을 계산하는 방법이 이해가지 않는다는 거다.
		XMFLOAT4X4 rot;
		rot._11 = mat._11 / scale.x;	rot._12 = mat._12 / scale.x;	rot._13 = mat._13 / scale.x;	rot._14 = 0.0f;
		rot._21 = mat._21 / scale.y;	rot._22 = mat._22 / scale.y;	rot._23 = mat._23 / scale.y;	rot._24 = 0.0f;
		rot._31 = mat._31 / scale.z;	rot._32 = mat._32 / scale.z;	rot._33 = mat._33 / scale.z;	rot._34 = 0.0f;
		rot._41 = 0.0f;					rot._42 = 0.0f;					rot._43 = 0.0f;					rot._44 = 1.0f;

		return XMQuaternionRotationMatrix(XMLoadFloat4x4(&rot));
	}

	void Transform::setRotation(const DirectX::FXMVECTOR& rotation)
	{
		if (DirectX::XMQuaternionEqual(rotation, getRotation()))
			return;

		// 부모의 월드 회전 쿼터니언의 역수를 곱한다.
		// 왜인지 까먹었다.
		setLocalPosition(HasParent() ?
			DirectX::XMQuaternionMultiply(rotation, DirectX::XMQuaternionInverse(GetParent()->getRotation()))
			: rotation);
	}

	DirectX::XMVECTOR Transform::getLocalRotation() const
	{
		return DirectX::XMLoadFloat4(&m_LocalRotation);
	}

	void Transform::setLocalRotation(const DirectX::FXMVECTOR& rotation)
	{
		if (DirectX::XMQuaternionEqual(rotation, getLocalRotation()))
			return;

		DirectX::XMStoreFloat4(&m_LocalRotation, rotation);

		UpdateTransform();
	}

	// 일단 World Scale 계산은 틀린 것 같다.
	DirectX::XMVECTOR Transform::getScale() const
	{
		if (HasParent())
		{
			auto scale = DirectX::XMVector3Transform(getLocalScale(), m_pParent->GetMatrix());
			return scale;
		}

		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	void Transform::setScale(const DirectX::FXMVECTOR& scale)
	{
		// 비교는 나중에

		if (HasParent())
		{
			// 부모가 있을 때
			// 일단 여기에서 역 곱하기
			setLocalScale(DirectX::XMVector3Transform(scale, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix())));
		}
		else
		{
			// 부모가 없을 때
			setLocalScale(scale);
		}
	}

	// 로컬은 그냥 가져와도 되나?
	DirectX::XMVECTOR Transform::getLocalScale() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	// 이건 다시 바꾸자.
	void Transform::setLocalScale(const DirectX::FXMVECTOR& scale)
	{
		DirectX::XMFLOAT3 destination;
		DirectX::XMStoreFloat3(&destination, scale);

		SetLocalScale(destination);
	}

	void Transform::translate(const DirectX::FXMVECTOR& translation, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			setPosition(getPosition() + translation);
			return;
		}
		case eSpace::Self:
		{
			// 회전행렬을 곱해야 한다.
			return;
		}
		default:
			return;
		}
	}
}