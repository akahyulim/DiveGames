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
	// �̰� ȣ���� �ȵǳ�...								//
	// GameObject Ȥ�� Scene ���Ű� ����� �ȵȴٴ� �Ҹ���.	//
	// �׸����� SceneManager������ �� ����.				//
	// SceneManager���� Clear ���� ����� �� �� ����.		//
	//======================================================//
	Transform::~Transform()
	{
		CORE_TRACE("Transform �Ҹ��� ȣ��");
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

	// ��ȯ ����� ������ �Ϸ��Ͽ���.
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

		// GetMatrix�� �θ��� Scale ��ȯ���� ����Ǵ� �ǵ�...
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
		// �̰͵� ��Ŀ��� ����ؾ� �Ѵ�.

		return DirectX::XMQuaternionIdentity();
	}
	
	void Transform::GetRotation(float& degreeX, float& degreeY, float& degreeZ) const
	{
		// �ִ� ����!!!

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
		// �̰� ��� ����ϳ�...

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
		// ��ȯ��Ŀ��� �����;� �ϴµ�...
		
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

		// 0.0f��� �ٸ� ������ ��������� �Ѵ�.

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
			// ȸ������� ���ؾ� �Ѵ�.
			//=> �̰� �ƴ� ���� ���� �� ����.
			// �� �� �����غ���.
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

	// ����� �ٶ󺸴� ������ ���溤�Ͱ� �ǵ��� ȸ����Ų��.
	// ����Ƽ������ �� ��° �Ű������� WorldUp vector�� ���� �޾Ҵ�.
	void Transform::LookAt(const DirectX::FXMVECTOR& worldPosition)
	{

	}

	void Transform::LookAt(float posX, float posY, float posZ)
	{
		LookAt(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}
}