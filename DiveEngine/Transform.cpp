#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileStream.h"

namespace dive
{
	Transform::Transform()
		: Component(typeid(Transform).hash_code())
	{
		mLocalPosition;

		mMatrix;
		mLocalMatrix;

		mLookAt;
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
		DirectX::XMMATRIX matLocalPos = DirectX::XMMatrixTranslationFromVector(getLocalPosition());
		DirectX::XMMATRIX matLocalRot = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX matLocalScl = DirectX::XMMatrixIdentity();

		auto localMatrix = matLocalScl * matLocalRot * matLocalPos;
		DirectX::XMStoreFloat4x4(&mLocalMatrix, localMatrix);

		if (HasParent())
		{
			DirectX::XMStoreFloat4x4(&mMatrix, (localMatrix * mParent->getMatrix()));
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

	DirectX::XMFLOAT3 Transform::GetPosition() const
	{
		return DirectX::XMFLOAT3(mMatrix._41, mMatrix._42, mMatrix._43);
	}

	void Transform::GetPosition(float& outX, float& outY, float& outZ)
	{
		outX = mMatrix._41;
		outY = mMatrix._42;
		outZ = mMatrix._43;
	}

	void Transform::GetLocalPosition(float& outX, float& outY, float& outZ)
	{
		outX = mLocalPosition.x;
		outY = mLocalPosition.y;
		outZ = mLocalPosition.z;
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		setPosition(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{;
		setLocalPosition(DirectX::XMLoadFloat3(&position));
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{;
		setLocalPosition(DirectX::XMVectorSet(x, y, z, 1.0f));
	}
	
	DirectX::XMVECTOR Transform::getPosition() const
	{
		DirectX::XMFLOAT3 pos = GetPosition();
		return DirectX::XMLoadFloat3(&pos);
	}
	
	DirectX::XMVECTOR Transform::getLocalPosition() const
	{
		return DirectX::XMLoadFloat3(&mLocalPosition);
	}

	void Transform::setPosition(const DirectX::XMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getPosition()))
			return;

		setLocalPosition( HasParent() ?
			DirectX::XMVector3Transform(position, DirectX::XMMatrixInverse(nullptr, mParent->getMatrix()))
			: position);
	}

	void Transform::setLocalPosition(const DirectX::XMVECTOR& position)
	{
		if (DirectX::XMVector3Equal(position, getLocalPosition()))
			return;

		DirectX::XMStoreFloat3(&mLocalPosition, position);
		UpdateTransform();
	}

	DirectX::XMMATRIX Transform::getMatrix() const
	{
		return DirectX::XMLoadFloat4x4(&mMatrix);
	}

	DirectX::XMMATRIX Transform::getLocalMatrix() const
	{
		return DirectX::XMLoadFloat4x4(&mLocalMatrix);
	}
}