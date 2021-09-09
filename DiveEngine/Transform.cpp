#include "Transform.h"
#include "DiveCore.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"
#include "dvMath.h"
#include "Log.h"

namespace dive
{
	
	Transform::Transform(GameObject* pGameObject)
		: Component(pGameObject, this)
	{
		// ���ĸ�ź�� AddComponent���� SetType()�� ���� Entity�� ���� �������ְ� �ִ�. ���ٰ�����... 
		m_Type = eComponentType::Transform;

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

	//=====================================================================//
	// ���� updateTransform�� ���ְ� IsChanged()�� �������� �����ؾ� �Ѵ�. //
	//=====================================================================//
	void Transform::Update(float deltaTime)
	{
		if (!HasChanged())
			return;

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
			// ���� ȣ���� �����ΰ�?
			child->m_bChanged = true;
			child->m_bChanged = true;
		}

		m_bChanged = false;
	}

	void Transform::Serialize(FileStream* pFileStream)
	{
		DV_ASSERT(pFileStream);

		pFileStream->Write(m_LocalPosition);
		pFileStream->Write(m_LocalRotation);
		pFileStream->Write(m_LocalScale);
		pFileStream->Write(m_pParent ? m_pParent->GetGameObject()->GetInstanceID() : 0);
	}

	void Transform::Deserialize(FileStream* pFileStream)
	{
		DV_ASSERT(pFileStream);

		pFileStream->Read(&m_LocalPosition);
		pFileStream->Read(&m_LocalRotation);
		pFileStream->Read(&m_LocalScale);
		unsigned int parentId = 0;
		pFileStream->Read(&parentId);

		m_bChanged = true;
	}

	void Transform::Clear()
	{
		m_LocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_LocalRotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		m_LocalScale	= DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

		m_bChanged = true;
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

		m_bChanged = true;
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		SetLocalPositionVector(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPositionVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	// Qaternion�� XMVECTOR Ÿ������ �����մϴ�.
	DirectX::XMVECTOR Transform::GetRotationVector() const
	{
		DirectX::XMVECTOR scale, rot, trans;
		DirectX::XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&m_Matrix));

		return rot;
	}

	// Qaternion�� XMFLOAT4 Ÿ������ �����մϴ�.
	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		DirectX::XMFLOAT4 rot;
		DirectX::XMStoreFloat4(&rot, GetRotationVector());

		return rot;
	}

	// ���Ϸ����� XMFLOAT3 Ÿ������ �����մϴ�.
	DirectX::XMFLOAT3 Transform::GetRotationEulerAngles() const
	{
		return QuaternionToEulerAngles(GetRotation());
	}
	
	void Transform::GetRotationEulerAngles(float& degreeX, float& degreeY, float& degreeZ) const
	{
		auto eularAngles = QuaternionToEulerAngles(GetRotation());

		degreeX = eularAngles.x;
		degreeY = eularAngles.y;
		degreeZ = eularAngles.z;
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

		m_bChanged = true;
	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT4& quaternion)
	{
		SetLocalRotationVector(DirectX::XMLoadFloat4(&quaternion));
	}

	void Transform::SetLocalRotationEulerAngles(const DirectX::XMFLOAT3& eularAngles)
	{
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

	// �ڽ��� ��� �ڽ��� ȸ���� ���� �������� Scale�Ǿ�� �ϴ� �� �ƴұ�?
	// �׽�Ʈ ��� �θ��� �࿡ ���� Scale �ȴ�.
	void Transform::SetLocalScaleVector(const DirectX::XMVECTOR& scale)
	{
		if (DirectX::XMVector3Equal(scale, GetLocalScaleVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalScale, scale);

		m_bChanged = true;
	}
	
	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{
		SetLocalScaleVector(DirectX::XMLoadFloat3(&scale));
	}

	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScaleVector(DirectX::XMVectorSet(x, y, z, 1.0f));
	}

	//==============================================================================//
	// �� ������ �´� �� ����.														//
	// ���� ��� Transform ��ü���� Update�� ���� �̵��ϹǷ� �̻��ϰ� ���̴� �Ŵ�.	//
	//==============================================================================//
	void Transform::TranslateVector(const DirectX::XMVECTOR& translation, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			SetLocalPositionVector(GetLocalPositionVector() + translation);
			return;
		}
		case eSpace::Self:
		{
			auto delta = DirectX::XMVector3Transform(translation, GetMatrix());
			SetPositionVector(delta);
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

	//==============================================//
	// �̰� �ϴ� ���߿� ��������. �׽�Ʈ�� �����.	//
	//==============================================//
	void Transform::TranslateVector(const DirectX::XMVECTOR& translation, const Transform* pRelativeTo)
	{
		if (pRelativeTo == nullptr)
		{
			SetLocalPositionVector(GetLocalPositionVector() + translation);
			return;
		}
		else
		{
			// pRelativeTo�� ��ǥ�迡 ���� translation �Ѵٴ� �ǹ����ٵ�...
			// �ᱹ�� �� ��ü�� ��ġ�� �̵����Ѿ� ���ݾ�.
			// �� �������ϸ� ��ġ���� ���ַ� �� �� ������...
			auto delta = DirectX::XMVector3Transform(translation, pRelativeTo->GetMatrix());
			
		}
	}

	void Transform::Translate(const DirectX::XMFLOAT3& translation, const Transform* pRelativeTo)
	{
		TranslateVector(DirectX::XMLoadFloat3(&translation), pRelativeTo);
	}

	void Transform::Translate(float x, float y, float z, const Transform* pRelativeTo)
	{
		TranslateVector(DirectX::XMVectorSet(x, y, z, 1.0f), pRelativeTo);
	}

	// �׽�Ʈ ���� ���ߴ�.
	void Transform::RotateVector(const DirectX::XMVECTOR& quaternion, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			// �׳� ���ϸ� �Ǵ� �ɱ�?
			auto delta = DirectX::XMQuaternionNormalize(GetLocalRotationVector() * quaternion);
			SetLocalRotationVector(delta);
			return;
		}
		case eSpace::Self:
		{
			// ���ĸ�ź��
			// '���� ȸ�� * ����ȸ�� �� ����� * ��Ÿ * ����ȸ�� �����'�� ����ߴ�.
			//SetLocalRotationVector(GetLocalRotationVector() * quaternion);
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

	// ���� Rotate�ʹ� �޸� ���������� �����ߴ�.
	// �ϴ� World�� ���ư��� �� ������ ������ �ָ��ϴ�.
	// Self�� �׽�Ʈ���� ���ߴ�.
	void Transform::RotateEulerAnglesVector(const DirectX::XMVECTOR& eularAngles, eSpace relativeTo)
	{
		// �������� �ϴ� �̷���...
		float radianX = DirectX::XMConvertToRadians(DirectX::XMVectorGetX(eularAngles));
		float radianY = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(eularAngles));
		float radianZ = DirectX::XMConvertToRadians(DirectX::XMVectorGetZ(eularAngles));

		// �Ű����� ������ �ٽ� Ȯ���ؾ� �Ѵ�.
		DirectX::XMVECTOR pitch = DirectX::XMQuaternionRotationRollPitchYaw(radianX, 0.0f, 0.0f);
		DirectX::XMVECTOR yaw = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, radianY, 0.0f);
		DirectX::XMVECTOR roll = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, radianZ);

		switch (relativeTo)
		{
		case eSpace::World:
		{
			DirectX::XMVECTOR rot = GetRotationVector();
			rot = DirectX::XMQuaternionMultiply(pitch, rot);
			rot = DirectX::XMQuaternionMultiply(rot, yaw);
			rot = DirectX::XMQuaternionMultiply(roll, rot);
			rot = DirectX::XMQuaternionNormalize(rot);

			SetRotationVector(rot);

			return;
		}
		case eSpace::Self:
		{
			DirectX::XMVECTOR rot = GetLocalRotationVector();
			rot = DirectX::XMQuaternionMultiply(pitch, rot);
			rot = DirectX::XMQuaternionMultiply(rot, yaw);
			rot = DirectX::XMQuaternionMultiply(roll, rot);
			rot = DirectX::XMQuaternionNormalize(rot);

			SetLocalRotationVector(rot);

			return;
		}
		default:
			return;
		}
	}

	void Transform::RotateEulerAngles(const DirectX::XMFLOAT3& eularAngles, eSpace relativeTo)
	{
		float radianX = DirectX::XMConvertToRadians(eularAngles.x);
		float radianY = DirectX::XMConvertToRadians(eularAngles.y);
		float radianZ = DirectX::XMConvertToRadians(eularAngles.z);

		// �Ű����� ������ �ٽ� Ȯ���ؾ� �Ѵ�.
		DirectX::XMVECTOR pitch = DirectX::XMQuaternionRotationRollPitchYaw(radianX, 0.0f, 0.0f);
		DirectX::XMVECTOR yaw = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, radianY, 0.0f);
		DirectX::XMVECTOR roll = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, radianZ);

		switch (relativeTo)
		{
		case eSpace::World:
		{
			DirectX::XMVECTOR rot = GetRotationVector();
			rot = DirectX::XMQuaternionMultiply(pitch, rot);
			rot = DirectX::XMQuaternionMultiply(rot, yaw);
			rot = DirectX::XMQuaternionMultiply(roll, rot);
			rot = DirectX::XMQuaternionIdentity();

			SetRotationVector(rot);

			return;
		}
		case eSpace::Self:
		{
			DirectX::XMVECTOR rot = GetLocalRotationVector();
			rot = DirectX::XMQuaternionMultiply(pitch, rot);
			rot = DirectX::XMQuaternionMultiply(rot, yaw);
			rot = DirectX::XMQuaternionMultiply(roll, rot);
			rot = DirectX::XMQuaternionIdentity();

			SetLocalRotationVector(rot);

			return;
		}
		default:
			return;
		}

	}

	void Transform::RotateEulerAngles(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
		RotateEulerAngles(DirectX::XMFLOAT3(degreeX, degreeY, degreeZ), relativeTo);
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

	DirectX::XMVECTOR Transform::ForwardVector() const
	{
		// ����� �� ������.
		// �ϴ� ���ʹϾ��� �̿��� float3�� ȸ����Ű�� ���
		// �ٸ� �ϳ��� ���ʹϾ����� ȸ�� ����� ���� ������ ȸ�� ��Ű�� ���
		// �ϴ� �� ������ ��ķ� �����غ���.
		return DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector()));
	}

	DirectX::XMFLOAT3 Transform::Forward() const
	{
		DirectX::XMFLOAT3 forward;
		DirectX::XMStoreFloat3(&forward, ForwardVector());

		return forward;
	}

	DirectX::XMVECTOR Transform::UpVector() const
	{
		// �ϴ� �̰͵� ��ķ� �����ߴ�.
		return DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector()));

		// ����� ����� ã�ƺ� �� �ٽ� ��������.
		// �ᱹ ����� ������ �� ���� �ؾ� �Ѵ�.
	}

	DirectX::XMFLOAT3 Transform::Up() const
	{
		DirectX::XMFLOAT3 up;
		DirectX::XMStoreFloat3(&up, UpVector());

		return up;
	}

	DirectX::XMVECTOR Transform::RightVector() const
	{
		return DirectX::XMVector3Transform(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMMatrixRotationQuaternion(GetLocalRotationVector()));
	}

	DirectX::XMFLOAT3 Transform::Right() const
	{
		DirectX::XMFLOAT3 right;
		DirectX::XMStoreFloat3(&right, RightVector());

		return right;
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
			child->m_bChanged = true;
		}
	}
}