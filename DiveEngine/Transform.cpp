#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"

namespace dive
{
	Transform::Transform(GameObject* owner)
		: Component(typeid(Transform).hash_code(), owner, this)
	{
		mLocalPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		DirectX::XMStoreFloat4x4(&mMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&mLocalMatrix, DirectX::XMMatrixIdentity());

		mLookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f); // 이건 추후 바꿔야 한다.
	}

	Transform::~Transform()
	{
	}

	void Transform::Update(float deltaTime)
	{
	}

	void Transform::Serialize(FileStream* fileStream)
	{
		// local pos, rot ,scl
		// lookAt
		fileStream->Write(mParent ? mParent->GetOwner()->GetID() : 0);
	}

	void Transform::Deserialize(FileStream* fileStream)
	{
		// local pos, rot ,scl
		// lookAt
		unsigned int parentId = 0;
		fileStream->Read(&parentId);

		if (parentId != 0)
		{
			if (auto parentTransform = Scene::GetGlobalScene().GetGameObjectByID(parentId))
			{
				SetParent(parentTransform->GetTransform());
			}
		}

		// update transform
	}

	void Transform::UpdateTransform()
	{
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(GetLocalPositionVector());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixIdentity();

		auto localMatrix = matLocalScl * matLocalRot * matLocalPos;
		DirectX::XMStoreFloat4x4(&mLocalMatrix, localMatrix);

		if (HasParent())
		{
			DirectX::XMStoreFloat4x4(&mMatrix, (localMatrix * mParent->GetMatrix()));
		}
		else
		{
			mMatrix = mLocalMatrix;
		}

		for (auto child : mChildren)
		{
			child->UpdateTransform();
		}
	}

	DirectX::XMVECTOR Transform::GetPositionVector() const
	{
		return DirectX::XMVectorSet(mMatrix._41, mMatrix._42, mMatrix._43, 1.0f);
	}

	DirectX::XMFLOAT3 Transform::GetPositionFloat3() const
	{
		return DirectX::XMFLOAT3(mMatrix._41, mMatrix._42, mMatrix._43);
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ) const
	{
		outX = mMatrix._41;
		outY = mMatrix._42;
		outZ = mMatrix._43;
	}

	DirectX::XMVECTOR Transform::GetLocalPositionVector() const
	{
		return DirectX::XMLoadFloat3(&mLocalPosition);
	}

	void Transform::GetLocalPosition(float& outX, float& outY, float& outZ) const
	{
		outX = mLocalPosition.x;
		outY = mLocalPosition.y;
		outZ = mLocalPosition.z;
	}

	void Transform::SetPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetPositionVector()))
			return;

		SetLocalPositionByVector(HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, mParent->GetMatrix()))
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

	void Transform::SetLocalPositionByVector(const DirectX::FXMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, GetLocalPositionVector()))
			return;

		DirectX::XMStoreFloat3(&mLocalPosition, position);

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

	void Transform::SetLookAt(float x, float y, float z)
	{
		mLookAt = DirectX::XMFLOAT3(x, y, z);
	}
}