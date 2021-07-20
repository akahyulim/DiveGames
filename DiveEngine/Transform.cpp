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
		m_LocalScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		DirectX::XMStoreFloat4x4(&m_Matrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&m_LocalMatrix, DirectX::XMMatrixIdentity());

		m_LookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // �̰� ���� �ٲ�� �Ѵ�.
	}

	//======================================================//
	// �̰� ȣ���� �ȵǳ�...								//
	// GameObject Ȥ�� Scene ���Ű� ����� �ȵȴٴ� �Ҹ���.	//
	// �׸����� SceneManager������ �� ����.				//
	//======================================================//
	Transform::~Transform()
	{
		CORE_TRACE("Transform �Ҹ��� ȣ��");
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

		// update transform
	}

	void Transform::UpdateTransform()
	{
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(GetLocalPositionVector());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixScalingFromVector(GetLocalScaleVector());

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

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return DirectX::XMVectorSet(m_Matrix._41, m_Matrix._42, m_Matrix._43, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPositionFloat3() const
	{
		//return DirectX::XMFLOAT3(m_Matrix._41, m_Matrix._42, m_Matrix._43);
		if (HasParent())
		{
			auto position = DirectX::XMVector3Transform(GetLocalPositionVector(), m_pParent->GetMatrix());
			DirectX::XMFLOAT3 result;
			DirectX::XMStoreFloat3(&result, position);
			return result;
		}
		else
		{
			return m_LocalPosition;
		}
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_Matrix._41;
		outY = m_Matrix._42;
		outZ = m_Matrix._43;
	}

	DirectX::XMVECTOR Transform::GetLocalPositionVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalPosition);
	}

	void Transform::GetLocalPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_LocalPosition.x;
		outY = m_LocalPosition.y;
		outZ = m_LocalPosition.z;
	}

	void Transform::SetPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetPositionVector()))
			return;

		SetLocalPositionByVector(HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix()))
			: position);
	}

	void Transform::SetPositionByFloat3(const DirectX::XMFLOAT3& position)
	{
		SetPositionByVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPositionByVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	// scaleó�� �ٲܰ��� �����ؾ� �Ѵ�.
	void Transform::SetLocalPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPositionVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		UpdateTransform();
	}

	void Transform::SetLocalPositionByFloat3(const DirectX::XMFLOAT3& position)
	{;
		SetLocalPositionByVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{;
		SetLocalPositionByVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	// �ϴ� World Scale ����� Ʋ�� �� ����.
	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		if (HasParent())
		{
			auto scale = DirectX::XMVector3Transform(GetLocalScaleVector(), m_pParent->GetMatrix());
			return scale;
		}

		return DirectX::XMLoadFloat3(&m_LocalScale);
	}
	
	// �ϴ� ���⿡�� �غ���
	DirectX::XMFLOAT3 Transform::GetScaleFloat3() const
	{
		if (HasParent())
		{
			auto scale = DirectX::XMVector3Transform(GetLocalScaleVector(), m_pParent->GetMatrix());
			DirectX::XMFLOAT3 result;
			DirectX::XMStoreFloat3(&result, scale);

			return result;
		}

		return m_LocalScale;
	}
	
	void Transform::GetScale(float& outX, float& outY, float& outZ)
	{
		auto scale = GetScaleFloat3();
		outX = scale.x;
		outY = scale.y;
		outZ = scale.z;
	}
	
	// ������ �׳� �����͵� �ǳ�?
	DirectX::XMVECTOR Transform::GetLocalScaleVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}

	void Transform::GetLocalScale(float& outX, float& outY, float& outZ)
	{
		outX = m_LocalScale.x;
		outY = m_LocalScale.y;
		outZ = m_LocalScale.z;
	}
	
	void Transform::SetScaleByVector(const DirectX::FXMVECTOR& scale)
	{
		// �񱳴� ���߿�

		if (HasParent())
		{
			// �θ� ���� ��
			// �ϴ� ���⿡�� �� ���ϱ�
			SetLocalScaleByVector(DirectX::XMVector3Transform(scale, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix())));
		}
		else
		{
			// �θ� ���� ��
			SetLocalScaleByVector(scale);
		}
	}
	
	void Transform::SetScaleByFloat3(const DirectX::XMFLOAT3& scale)
	{
		SetScaleByVector(DirectX::XMLoadFloat3(&scale));
	}
	
	void Transform::SetScale(float x, float y, float z)
	{
	}
	
	void Transform::SetLocalScaleByVector(const DirectX::FXMVECTOR& scale)
	{
		DirectX::XMFLOAT3 destination;
		DirectX::XMStoreFloat3(&destination, scale);

		SetLocalScaleByFloat3(destination);
	}
	
	void Transform::SetLocalScaleByFloat3(const DirectX::XMFLOAT3& scale)
	{
		// ����... 0.0�̾ �ȵǴµ�

		if ((m_LocalScale.x == scale.x) && (m_LocalScale.y == scale.y) && (m_LocalScale.z == scale.z))
			return;

		m_LocalScale = scale;

		UpdateTransform();
	}
	
	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScaleByFloat3(DirectX::XMFLOAT3(x, y, z ));
	}

	void Transform::SetLookAt(float x, float y, float z)
	{
		m_LookAt = DirectX::XMFLOAT3(x, y, z);
	}
}