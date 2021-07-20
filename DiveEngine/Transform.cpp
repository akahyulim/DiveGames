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

		m_LookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // �̰� ���� �ٲ�� �Ѵ�.
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

	// �̰� �ٽ� setLocalScaleByVector�� �ٲ���
	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		// ����... 0.0�̾ �ȵǴµ�

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

	// scaleó�� �ٲܰ��� �����ؾ� �Ѵ�.
	void Transform::setLocalPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getLocalPositionByVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		UpdateTransform();
	}

	// �ϴ� World Scale ����� Ʋ�� �� ����.
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
		// �񱳴� ���߿�

		if (HasParent())
		{
			// �θ� ���� ��
			// �ϴ� ���⿡�� �� ���ϱ�
			setLocalScaleByVector(DirectX::XMVector3Transform(scale, DirectX::XMMatrixInverse(nullptr, m_pParent->GetMatrix())));
		}
		else
		{
			// �θ� ���� ��
			setLocalScaleByVector(scale);
		}
	}

	// ������ �׳� �����͵� �ǳ�?
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