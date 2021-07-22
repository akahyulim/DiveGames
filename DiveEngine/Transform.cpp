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
		//Translate(DirectX::XMVectorSet(0.0f, 0.1f, 0.0f, 1.0f) * deltaTime, eSpace::World);
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
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(GetLocalPosition());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixRotationQuaternion(GetLocalRotation());
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixScalingFromVector(GetLocalScale());

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

	DirectX::XMVECTOR Transform::GetPosition() const
	{
		return DirectX::XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_Matrix._41;
		outY = m_Matrix._42;
		outZ = m_Matrix._43;
	}

	void Transform::SetPosition(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetPosition()))
			return;

		// GetMatrix면 부모의 Scale 변환까지 적용되는 건데...
		SetLocalPosition(
			HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, GetParent()->GetMatrix())) :
			position
		);
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	DirectX::XMVECTOR Transform::GetLocalPosition() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	void Transform::GetLocalPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_LocalPosition.x;
		outY = m_LocalPosition.y;
		outZ = m_LocalPosition.z;
	}

	void Transform::SetLocalPosition(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPosition()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		UpdateTransform();
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	DirectX::XMVECTOR Transform::GetRotation() const
	{
		// 이것두 행렬에서 계산해야 한다.

		return DirectX::XMQuaternionIdentity();
	}
	
	void Transform::GetRotation(float& degreeX, float& degreeY, float& degreeZ) const
	{
		// 최대 난관!!!

		degreeX = 0.0f;
		degreeY = 0.0f;
		degreeZ = 0.0f;
	}
	
	void Transform::SetRotation(const DirectX::FXMVECTOR& quaternion)
	{
		if (DirectX::XMQuaternionEqual(quaternion, GetRotation()))
			return;

		SetLocalRotation(
			HasParent() ?
			quaternion * DirectX::XMQuaternionInverse(GetParent()->GetRotation()) :
			quaternion
		);
	}
	
	void Transform::SetRotation(float degreeX, float degreeY, float degreeZ)
	{
		float radianX = DirectX::XMConvertToRadians(degreeX);
		float radianY = DirectX::XMConvertToRadians(degreeY);
		float radianZ = DirectX::XMConvertToRadians(degreeZ);

		SetRotation(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}
	
	DirectX::XMVECTOR Transform::GetLocalRotation() const
	{
		return DirectX::XMLoadFloat4(&m_LocalRotation);
	}
	
	void Transform::GetLocalRotation(float& degreeX, float& degreeY, float& degreeZ) const
	{
		// 이걸 어떻게 계산하냐...

		degreeX = 0.0f;
		degreeY = 0.0f;
		degreeZ = 0.0f;
	}
	
	void Transform::SetLocalRotation(const DirectX::FXMVECTOR& quaternion)
	{
		if (DirectX::XMQuaternionEqual(quaternion, GetLocalRotation()))
			return;

		DirectX::XMStoreFloat4(&m_LocalRotation, quaternion);

		UpdateTransform();
	}
	
	void Transform::SetLocalRotation(float degreeX, float degreeY, float degreeZ)
	{
		float radianX = DirectX::XMConvertToRadians(degreeX);
		float radianY = DirectX::XMConvertToRadians(degreeY);
		float radianZ = DirectX::XMConvertToRadians(degreeZ);

		SetLocalRotation(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}
	
	DirectX::XMVECTOR Transform::GetScale() const
	{
		// 변환행렬에서 가져와야 하는데...
		
		return DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	}

	void Transform::GetScale(float& outX, float& outY, float& outZ) const
	{
		auto scale = GetScale();

		outX = DirectX::XMVectorGetX(scale);
		outY = DirectX::XMVectorGetY(scale);
		outZ = DirectX::XMVectorGetZ(scale);
	}
	
	void Transform::SetScale(const DirectX::FXMVECTOR& scale)
	{
		if (DirectX::XMVector3Equal(scale, GetScale()))
			return;

		SetLocalScale(
			HasParent() ?
			scale / GetParent()->GetScale() :
			scale
		);
	}
	
	void Transform::SetScale(float x, float y, float z)
	{
		SetScale(DirectX::XMVectorSet(x, y, z, 1.0f));
	}
	
	DirectX::XMVECTOR Transform::GetLocalScale() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}
	
	void Transform::GetLocalScale(float& outX, float& outY, float& outZ) const
	{
		outX = m_LocalScale.x;
		outY = m_LocalScale.y;
		outZ = m_LocalScale.z;
	}
	
	void Transform::SetLocalScale(const DirectX::FXMVECTOR& scale)
	{
		if (DirectX::XMVector3Equal(scale, GetLocalScale()))
			return;

		DirectX::XMStoreFloat3(&m_LocalScale, scale);

		// 0.0f라면 다른 값으로 변경해줘야 한다.

		UpdateTransform();
	}
	
	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScale(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	void Transform::Translate(const DirectX::FXMVECTOR& translation, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			SetPosition(GetPosition() + translation);
			return;
		}
		case eSpace::Self:
		{
			// 회전행렬을 곱해야 한다.
			//=> 이게 아닐 수도 있을 것 같다.
			// 좀 더 생각해보자.
			return;
		}
		default:
			return;
		}
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(DirectX::XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	void Transform::Rotate(const DirectX::FXMVECTOR& enulerAngles, eSpace relativeTo)
	{
	}

	void Transform::Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
	}

	void Transform::LookAt(const Transform& target)
	{
		LookAt(target.GetPosition());
	}

	// 대상을 바라보는 방향이 전방벡터가 되도록 회전시킨다.
	// 유니티에서는 두 번째 매개변수로 WorldUp vector를 전달 받았다.
	void Transform::LookAt(const DirectX::FXMVECTOR& worldPosition)
	{

	}

	void Transform::LookAt(float posX, float posY, float posZ)
	{
		LookAt(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}
}