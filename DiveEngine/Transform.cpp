#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"
#include "dvMath.h"
#include "Log.h"

namespace dive
{
	
	Transform::Transform(GameObject* pGameObject)
		: Component(typeid(Transform).hash_code(), pGameObject, this)
	{
		Clear();
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
	}

	void Transform::Update(float deltaTime)
	{
		//test
		TranslateVector(DirectX::XMVectorSet(0.0f, 0.1f, 0.0f, 1.0f) * deltaTime);
	}

	void Transform::Serialize(FileStream* pFileStream)
	{
		pFileStream->Write(m_LocalPosition);
		pFileStream->Write(m_LocalRotation);
		pFileStream->Write(m_LocalScale);
		pFileStream->Write(m_pParent ? m_pParent->GetGameObject()->GetInstanceID() : 0);
	}

	void Transform::Deserialize(FileStream* pFileStream)
	{
		pFileStream->Read(&m_LocalPosition);
		pFileStream->Read(&m_LocalRotation);
		pFileStream->Read(&m_LocalScale);
		unsigned int parentId = 0;
		pFileStream->Read(&parentId);

		updateTransform();
	}

	void Transform::Clear()
	{
		m_LocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale	= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		updateTransform();
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(&m_Matrix._41));
	}

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return *((DirectX::XMFLOAT3*)&m_Matrix._41);
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ) const
	{
		outX = m_Matrix._41;
		outY = m_Matrix._42;
		outZ = m_Matrix._43;
	}

	void Transform::SetPositionVector(const DirectX::XMVECTOR& position)
	{
		SetLocalPositionVector(
			HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, GetParent()->GetMatrix())) :
			position
		);
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		SetPositionVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPositionVector(DirectX::XMVectorSet(x, y, z, 1.0f));
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

	void Transform::SetLocalPositionVector(const DirectX::XMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPositionVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalPosition, position);

		updateTransform();
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		SetLocalPositionVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPositionVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	DirectX::XMVECTOR Transform::GetRotationVector() const
	{
		DirectX::XMVECTOR scale, rot, trans;
		DirectX::XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&m_Matrix));

		return rot;
	}

	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		DirectX::XMFLOAT4 rot;
		DirectX::XMStoreFloat4(&rot, GetRotationVector());

		return rot;
	}
	
	void Transform::GetRotationEulerAngles(float& degreeX, float& degreeY, float& degreeZ) const
	{
		// GetRotation�� ����� ����ؾ� �Ѵ�.

		degreeX = 0.0f;
		degreeY = 0.0f;
		degreeZ = 0.0f;
	}
	
	void Transform::SetRotationVector(const DirectX::XMVECTOR& quaternion)
	{
		SetLocalRotationVector(
			HasParent() ?
			DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionInverse(GetParent()->GetRotationVector())) :
			quaternion
		);
	}

	void Transform::SetRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetRotationVector(DirectX::XMLoadFloat4(&quaternion));
	}

	void Transform::SetRotationEulerAngles(const DirectX::XMFLOAT3& eularAngles)
	{
		// �ϴ� �Ʒ� ����
		float radianX = DirectX::XMConvertToRadians(eularAngles.x);
		float radianY = DirectX::XMConvertToRadians(eularAngles.y);
		float radianZ = DirectX::XMConvertToRadians(eularAngles.z);

		SetRotationVector(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}
	
	void Transform::SetRotationEulerAngles(float degreeX, float degreeY, float degreeZ)
	{
		float radianX = DirectX::XMConvertToRadians(degreeX);
		float radianY = DirectX::XMConvertToRadians(degreeY);
		float radianZ = DirectX::XMConvertToRadians(degreeZ);

		// ��� �� �κе� ���ذ� �ʿ��ϴ�. �׳� �Լ��� ������ ���ٰ� �ذ�Ǵ� ������ �ƴϴ�.
		SetRotationVector(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}
	
	DirectX::XMVECTOR Transform::GetLocalRotationVector() const
	{
		return DirectX::XMLoadFloat4(&m_LocalRotation);
	}
	
	DirectX::XMFLOAT3 Transform::GetLocalRotationEulerAngles() const
	{
		return QuaternionToEulerAngles(m_LocalRotation);
	}

	void Transform::GetLocalRotationEulerAngles(float& degreeX, float& degreeY, float& degreeZ) const
	{
		auto eulerAngles = QuaternionToEulerAngles(m_LocalRotation);

		degreeX = eulerAngles.x;
		degreeY = eulerAngles.y;
		degreeZ = eulerAngles.z;
	}
	
	void Transform::SetLocalRotationVector(const DirectX::XMVECTOR& quaternion)
	{
		if (DirectX::XMQuaternionEqual(quaternion, GetLocalRotationVector()))
			return;

		DirectX::XMStoreFloat4(&m_LocalRotation, quaternion);

		updateTransform();
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationVector(DirectX::XMLoadFloat4(&quaternion));
	}

	void Transform::SetLocalRotationEulerAngles(const DirectX::XMFLOAT3& eularAngles)
	{
		// �ϴ� �Ʒ� ����
		float radianX = DirectX::XMConvertToRadians(eularAngles.x);
		float radianY = DirectX::XMConvertToRadians(eularAngles.y);
		float radianZ = DirectX::XMConvertToRadians(eularAngles.z);

		SetLocalRotationVector(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}
	
	void Transform::SetLocalRotationEulerAngles(float degreeX, float degreeY, float degreeZ)
	{
		float radianX = DirectX::XMConvertToRadians(degreeX);
		float radianY = DirectX::XMConvertToRadians(degreeY);
		float radianZ = DirectX::XMConvertToRadians(degreeZ);

		SetLocalRotationVector(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}

	DirectX::XMVECTOR Transform::GetScaleVector() const
	{
		DirectX::XMVECTOR scale, rot, trans;
		DirectX::XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&m_Matrix));
		
		return scale;
	}

	DirectX::XMFLOAT3 Transform::GetScale() const
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMStoreFloat3(&scale, GetScaleVector());

		return scale;
	}

	void Transform::GetScale(float& outX, float& outY, float& outZ) const
	{
		auto scale = GetScale();

		outX = scale.x;
		outY = scale.y;
		outZ = scale.z;
	}
	
	void Transform::SetScaleVector(const DirectX::XMVECTOR& scale)
	{
		SetLocalScaleVector(
			HasParent() ?
			scale / GetParent()->GetScaleVector() :
			scale
		);
	}
	
	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{
		SetScaleVector(DirectX::XMLoadFloat3(&scale));
	}

	void Transform::SetScale(float x, float y, float z)
	{
		SetScaleVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}
	
	DirectX::XMVECTOR Transform::GetLocalScaleVector() const
	{
		return DirectX::XMLoadFloat3(&m_LocalScale);
	}
	
	void Transform::GetLocalScale(float& outX, float& outY, float& outZ) const
	{
		outX = m_LocalScale.x;
		outY = m_LocalScale.y;
		outZ = m_LocalScale.z;
	}
	
	void Transform::SetLocalScaleVector(const DirectX::XMVECTOR& scale)
	{
		if (DirectX::XMVector3Equal(scale, GetLocalScaleVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalScale, scale);

		// ��� �� �ϳ��� 0.0f �� �ȵǱ� ������ Ȯ�� �� ������ �־�� �Ѵ�.

		updateTransform();
	}
	
	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		SetLocalScaleVector(DirectX::XMLoadFloat3(&scale));
	}

	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScaleVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	// �׽�Ʈ �ϱⰡ �ָ��ϴ�.
	// ���� ���ο��� updateTransform()�� ȣ���ϰ� �ִ�.
	void Transform::TranslateVector(const DirectX::XMVECTOR& translation, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			// �̰� ������ �����ߴµ� �׽�Ʈ �غ��� ���� ��ǥ�� �����δ�.
			SetPositionVector(GetPositionVector() + translation);
			return;
		}
		case eSpace::Self:
		{
			// �Ƹ��� translation�� ���� ȸ���� ���ؾ� �� �� ����.
			return;
		}
		default:
			return;
		}
	}

	void Transform::Translate(const DirectX::XMFLOAT3& translation, eSpace relativeTo)
	{
		TranslateVector(DirectX::XMLoadFloat3(&translation), relativeTo);
	}

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		TranslateVector(DirectX::XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	// ���� ���Ϸ� �ޱ��� DirectX::XMFLOAT3�� �޴� �Լ���.
	// �׷��� XMVECTOR�� ��� �ָ��ؼ� �׳� Quaternion���� �ξ���.
	// �̴� Rotation�� ����������. ������ ���Ϸ� �ޱ��� ������ Vec3�� �޴°� �ڿ������� ������ ���̴�.
	// �׷��� �� Rotate�� ��� XMFLOAT3�� ������ deltaTime�� ���� ���� ����.
	void Transform::RotateVector(const DirectX::XMVECTOR& quaternion, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			return;
		}
		case eSpace::Self:
		{
			return;
		}
		default:
			return;
		}
	}

	void Transform::Rotate(const DirectX::XMFLOAT4& quaternion, eSpace relativeTo)
	{
		RotateVector(DirectX::XMLoadFloat4(&quaternion), relativeTo);
	}

	void Transform::RotateEulerAngles(const DirectX::XMFLOAT3& eularAngles, eSpace relativeTo)
	{
		// �̰Ͱ� �Ʒ����� �ϴ� ������ ������ ���̴�.
		// �׷��� Wicked���� ������ �̿��� ȸ�� ���ʹϾ��� ���� ���� ���� ���� �����ϴ°� �ƴϴ�.
		// �� �� ���� ȸ�� ������� ���� ��
		// ���ϴ� ������ Ư�� ��Ģ�� ������.
		// �׸��� ���������� ȸ�� ������� ����ȭ�ߴ�.
	}

	void Transform::RotateEulerAngles(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
	}

	void Transform::LookAt(const Transform& target)
	{
		LookAt(target.GetPositionVector());
	}

	// ����� �ٶ󺸴� ������ ���溤�Ͱ� �ǵ��� ȸ����Ų��.
	// ����Ƽ������ �� ��° �Ű������� WorldUp vector�� ���� �޾Ҵ�.
	// �̰͵� ���ó� DirectX::XMFLOAT3�� �ڿ������� ���̴�.
	void Transform::LookAt(const DirectX::XMVECTOR& worldPosition)
	{

	}

	void Transform::LookAt(float posX, float posY, float posZ)
	{
		LookAt(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void Transform::updateTransform()
	{
		// ���� ȣ���ϴ� ���� �ƴϸ�
		// Update�� �����ϰ�
		// SetDirty�� ���� ���θ� �������־�� �Ѵ�.

		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(GetLocalPositionVector());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector());
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
			child->updateTransform();
		}
	}
}