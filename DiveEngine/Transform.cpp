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
	// 되긴 하는데... 버그가 있다.
	// 그리고 위치가 애매하다.
	// 그냥 math를 직접 만들까 싶다...
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
	// 이게 호출이 안되네...								//
	// GameObject 혹은 Scene 제거가 제대로 안된다는 소리다.	//
	// 그리고보니 SceneManager때문인 것 같다.				//
	// SceneManager에서 Clear 등을 만들면 될 것 같다.		//
	//======================================================//
	Transform::~Transform()
	{
		CORE_TRACE("Transform 소멸자 호출");
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
		// 아... 행렬에서 가져올 수 밖에 없나...
		// position의 경우엔 가장 깔끔하긴 하다.
		// 그런데 유니티에선 관련 함수가 없다.

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

	//= 여기서 부터 ============================================================================
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
	//= 여기 까지 ===================================================================================
	
	Quaternion Transform::dvGetRotation() const
	{
		// 이것두 역시 행렬로 부터 구해야 하나...
		return Quaternion();
	}

	void Transform::dvSetRotation(const Quaternion& q)
	{
		if (dvGetRotation() == q)
			return;

		// 회전 사원수 역을 곱한다. 아직 미구현
	}

	void Transform::dvSetLocalRotation(const Quaternion& q)
	{
		if (m_dvLocalRotation == q)
			return;

		m_dvLocalRotation = q;

		transformUpdate();
	}

	//= 여기서 부터 ========================================================================================================
	DirectX::XMVECTOR Transform::GetRotation() const
	{
		// 행렬에서 계산해야 한다.

		return DirectX::XMQuaternionIdentity();
	}
	
	void Transform::GetRotation(float& degreeX, float& degreeY, float& degreeZ) const
	{
		// GetRotation의 결과를 사용해야 한다.

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

		// 사실 이 부분도 이해가 필요하다. 그냥 함수만 가져다 쓴다고 해결되는 문제가 아니다.
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
	// 여기까지 ============================================================================================================

	Vector3 Transform::dvGetScale() const
	{
		// 이것두 행렬에서 가져와야 한다.
		// 그런데 유니티는 어떻게 했을까...

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
	
	//= 여기에서 부터 =========================================================================================
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

		// 요소 중 하나라도 0.0f 면 안되기 때문이 확인 후 변경해 주어야 한다.

		UpdateTransform();
	}
	
	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScale(DirectX::XMVectorSet(x, y, z, 1.0f));
	}
	//= 여기까지 ===============================================================================================================

	// 테스트 하기가 애매하다.
	void Transform::Translate(const DirectX::FXMVECTOR& translation, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			// 이게 셀프라 생각했는데 테스트 해보니 월드 좌표로 움직인다.
			SetPosition(GetPosition() + translation);
			return;
		}
		case eSpace::Self:
		{
			// 아마도 translation에 월드 회전을 곱해야 할 거 같다.
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

	// 원래 오일러 앵글을 Vector3로 받는 함수다.
	// 그런데 XMVECTOR의 경우 애매해서 그냥 Quaternion으로 두었다.
	// 이는 Rotation도 마찬가지다. 원래는 오일러 앵글의 각도를 Vec3로 받는게 자연스러운 구현일 것이다.
	// 그런데 이 Rotate의 경우 XMFLOAT3로 받으면 deltaTime을 곱할 수가 없다.
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

	// 대상을 바라보는 방향이 전방벡터가 되도록 회전시킨다.
	// 유니티에서는 두 번째 매개변수로 WorldUp vector를 전달 받았다.
	// 이것도 역시나 Vector3가 자연스러울 것이다.
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