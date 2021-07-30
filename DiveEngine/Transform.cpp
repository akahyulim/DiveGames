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
	// 이게 호출이 안되네...								//
	// GameObject 혹은 Scene 제거가 제대로 안된다는 소리다.	//
	// 그리고보니 SceneManager때문인 것 같다.				//
	// SceneManager에서 Clear 등을 만들면 될 것 같다.		//
	//======================================================//
	Transform::~Transform()
	{
		CORE_TRACE("Transform 소멸자 호출");
	}

	//=====================================================================//
	// 추후 updateTransform을 없애고 IsChanged()를 적용할지 결정해야 한다. //
	//=====================================================================//
	void Transform::Update(float deltaTime)
	{
		// 미리 언질을 해놓자.
		// Editor에서는 기본적으로 Update를 꺼놓아야 한다.
		// 그리고 테스트 실행시 Update를 수행한다.
		// 그런데 IsChanged()로 Transform을 Update한다면 즉각적으로 Editor에 반영되지 않는다.

		// 이 곳에서 Translate, Rotate를 test하면 각자 적용되기 때문에 결과 예측이 너무 복잡해진다.
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

	// Qaternion을 XMVECTOR 타입으로 리턴합니다.
	DirectX::XMVECTOR Transform::GetRotationVector() const
	{
		DirectX::XMVECTOR scale, rot, trans;
		DirectX::XMMatrixDecompose(&scale, &rot, &trans, XMLoadFloat4x4(&m_Matrix));

		return rot;
	}

	// Qaternion을 XMFLOAT4 타입으로 리턴합니다.
	DirectX::XMFLOAT4 Transform::GetRotation() const
	{
		DirectX::XMFLOAT4 rot;
		DirectX::XMStoreFloat4(&rot, GetRotationVector());

		return rot;
	}

	// 오일러각을 XMFLOAT3 타입으로 리턴합니다.
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

		updateTransform();
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

	// 자식의 경우 자신의 회전된 축을 기준으로 Scale되어야 하는 거 아닐까?
	// 테스트 결과 부모의 축에 따라 Scale 된다.
	void Transform::SetLocalScaleVector(const DirectX::XMVECTOR& scale)
	{
		if (DirectX::XMVector3Equal(scale, GetLocalScaleVector()))
			return;

		DirectX::XMStoreFloat3(&m_LocalScale, scale);

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

	//==============================================================================//
	// 이 구현이 맞느 것 같다.														//
	// 현재 모든 Transform 객체들이 Update를 통해 이동하므로 이상하게 보이는 거다.	//
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
	// 이건 일단 나중에 구현하자. 테스트가 힘들다.	//
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
			// pRelativeTo의 좌표계에 맞춰 translation 한다는 의미일텐데...
			// 결국엔 이 객체의 위치를 이동시켜야 하잖아.
			// 또 막구현하면 위치값이 우주로 갈 거 같은데...
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

	void Transform::RotateVector(const DirectX::XMVECTOR& quaternion, eSpace relativeTo)
	{
		switch (relativeTo)
		{
		case eSpace::World:
		{
			// 그냥 곱하면 되는 걸까?
			auto delta = DirectX::XMQuaternionNormalize(GetLocalRotationVector() * quaternion);
			SetLocalRotationVector(delta);
			return;
		}
		case eSpace::Self:
		{
			// 스파르탄은
			// '로컬 회전 * 월드회전 역 사원수 * 델타 * 월드회전 사원수'로 계산했다.
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

	void Transform::RotateEulerAnglesVector(const DirectX::XMVECTOR& eularAngles, eSpace relativeTo)
	{
		// 에바지만 일단 이렇게...
		float radianX = DirectX::XMConvertToRadians(DirectX::XMVectorGetX(eularAngles));
		float radianY = DirectX::XMConvertToRadians(DirectX::XMVectorGetY(eularAngles));
		float radianZ = DirectX::XMConvertToRadians(DirectX::XMVectorGetZ(eularAngles));

		// 매개변수 순서를 다시 확인해야 한다.
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

		// 매개변수 순서를 다시 확인해야 한다.
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

	// 대상을 바라보는 방향이 전방벡터가 되도록 회전시킨다.
	// 유니티에서는 두 번째 매개변수로 WorldUp vector를 전달 받았다.
	// 이것도 역시나 DirectX::XMFLOAT3가 자연스러울 것이다.
	void Transform::LookAt(const DirectX::XMVECTOR& worldPosition)
	{

	}

	void Transform::LookAt(float posX, float posY, float posZ)
	{
		LookAt(DirectX::XMVectorSet(posX, posY, posZ, 1.0f));
	}

	void Transform::updateTransform()
	{
		// 직접 호출하는 것이 아니면
		// Update에 구현하고
		// SetDirty로 변경 여부를 설정해주어야 한다.

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