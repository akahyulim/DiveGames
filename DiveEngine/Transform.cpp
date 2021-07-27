#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"
#include "Log.h"
//#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace dive
{
	// �Ǳ� �ϴµ�... ���װ� �ִ�.
	// �׸��� ��ġ�� �ָ��ϴ�.
	// �׳� math�� ���� ����� �ʹ�...
	void QuaternionToEulerAngles(DirectX::XMFLOAT4 q, float& degreeX, float& degreeY, float& degreeZ)
	{
		float roll, pitch, yaw;

		float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
		float cosr_cosp = 1.0f - 2 * (q.x * q.x + q.y * q.y);
		roll = std::atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		float sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
			pitch = std::asin(sinp);

		// yaw (z-axis rotation)
		float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		yaw = std::atan2(siny_cosp, cosy_cosp);

		degreeX = DirectX::XMConvertToDegrees(roll);
		degreeY = DirectX::XMConvertToDegrees(pitch);
		degreeZ = DirectX::XMConvertToDegrees(yaw);
	}

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
	}

	void Transform::Update(float deltaTime)
	{
		//test
		Translate(DirectX::XMVectorSet(0.0f, 0.1f, 0.0f, 1.0f) * deltaTime);
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

		UpdateTransform();
	}

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

	Vector3 Transform::dvGetPosition() const
	{
		// ��... ��Ŀ��� ������ �� �ۿ� ����...
		// position�� ��쿣 ���� ����ϱ� �ϴ�.
		// �׷��� ����Ƽ���� ���� �Լ��� ����.

		return Vector3::Zero;
	}

	void Transform::dvSetPosition(const Vector3& p)
	{
		if (dvGetPosition() == p)
			return;

		// SetLocal...
	}

	void Transform::dvSetLocalPosition(const Vector3& p)
	{
		if (m_dvLocalPosition == p)
			return;

		m_dvLocalPosition = p;

		transformUpdate();
	}

	//= ���⼭ ���� ============================================================================
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
	//= ���� ���� ===================================================================================
	
	Quaternion Transform::dvGetRotation() const
	{
		// �̰͵� ���� ��ķ� ���� ���ؾ� �ϳ�...
		return Quaternion();
	}

	void Transform::dvSetRotation(const Quaternion& q)
	{
		if (dvGetRotation() == q)
			return;

		// ȸ�� ����� ���� ���Ѵ�. ���� �̱���
	}

	void Transform::dvSetLocalRotation(const Quaternion& q)
	{
		if (m_dvLocalRotation == q)
			return;

		m_dvLocalRotation = q;

		transformUpdate();
	}

	//= ���⼭ ���� ========================================================================================================
	DirectX::XMVECTOR Transform::GetRotation() const
	{
		// ��Ŀ��� ����ؾ� �Ѵ�.

		return DirectX::XMQuaternionIdentity();
	}
	
	void Transform::GetRotation(float& degreeX, float& degreeY, float& degreeZ) const
	{
		// GetRotation�� ����� ����ؾ� �Ѵ�.

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
			DirectX::XMQuaternionMultiply(quaternion, DirectX::XMQuaternionInverse(GetParent()->GetRotation())) :
			quaternion
		);
	}
	
	void Transform::SetRotation(float degreeX, float degreeY, float degreeZ)
	{
		float radianX = DirectX::XMConvertToRadians(degreeX);
		float radianY = DirectX::XMConvertToRadians(degreeY);
		float radianZ = DirectX::XMConvertToRadians(degreeZ);

		// ��� �� �κе� ���ذ� �ʿ��ϴ�. �׳� �Լ��� ������ ���ٰ� �ذ�Ǵ� ������ �ƴϴ�.
		SetRotation(DirectX::XMQuaternionRotationRollPitchYaw(radianX, radianY, radianZ));
	}
	
	DirectX::XMVECTOR Transform::GetLocalRotation() const
	{
		return DirectX::XMLoadFloat4(&m_LocalRotation);
	}
	
	void Transform::GetLocalRotation(float& degreeX, float& degreeY, float& degreeZ) const
	{
		QuaternionToEulerAngles(m_LocalRotation, degreeX, degreeY, degreeZ);
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
	// ������� ============================================================================================================

	Vector3 Transform::dvGetScale() const
	{
		// �̰͵� ��Ŀ��� �����;� �Ѵ�.
		// �׷��� ����Ƽ�� ��� ������...

		return Vector3();
	}

	void Transform::dvSetScale(const Vector3& s)
	{
		if (dvGetScale() == s)
			return;

		dvSetLocalScale(
			HasParent() ?
			s / GetParent()->dvGetScale() : s);
	}

	void Transform::dvSetLocalScale(const Vector3& s)
	{
		if (m_dvLocalScale == s)
			return;

		m_dvLocalScale = s;

		transformUpdate();
	}
	
	//= ���⿡�� ���� =========================================================================================
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

		// ��� �� �ϳ��� 0.0f �� �ȵǱ� ������ Ȯ�� �� ������ �־�� �Ѵ�.

		UpdateTransform();
	}
	
	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScale(DirectX::XMVectorSet(x, y, z, 1.0f));
	}
	//= ������� ===============================================================================================================

	// �׽�Ʈ �ϱⰡ �ָ��ϴ�.
	void Transform::Translate(const DirectX::FXMVECTOR& translation, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			// �̰� ������ �����ߴµ� �׽�Ʈ �غ��� ���� ��ǥ�� �����δ�.
			SetPosition(GetPosition() + translation);
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

	void Transform::Translate(float x, float y, float z, eSpace relativeTo)
	{
		Translate(DirectX::XMVectorSet(x, y, z, 1.0f), relativeTo);
	}

	// ���� ���Ϸ� �ޱ��� Vector3�� �޴� �Լ���.
	// �׷��� XMVECTOR�� ��� �ָ��ؼ� �׳� Quaternion���� �ξ���.
	// �̴� Rotation�� ����������. ������ ���Ϸ� �ޱ��� ������ Vec3�� �޴°� �ڿ������� ������ ���̴�.
	// �׷��� �� Rotate�� ��� XMFLOAT3�� ������ deltaTime�� ���� ���� ����.
	void Transform::Rotate(const DirectX::FXMVECTOR& quaternion, eSpace relativeTo)
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

	void Transform::Rotate(float degreeX, float degreeY, float degreeZ, eSpace relativeTo)
	{
	}

	void Transform::LookAt(const Transform& target)
	{
		LookAt(target.GetPosition());
	}

	// ����� �ٶ󺸴� ������ ���溤�Ͱ� �ǵ��� ȸ����Ų��.
	// ����Ƽ������ �� ��° �Ű������� WorldUp vector�� ���� �޾Ҵ�.
	// �̰͵� ���ó� Vector3�� �ڿ������� ���̴�.
	void Transform::LookAt(const DirectX::FXMVECTOR& worldPosition)
	{

	}

	void Transform::LookAt(float posX, float posY, float posZ)
	{
		LookAt(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void Transform::transformUpdate()
	{
	}
}