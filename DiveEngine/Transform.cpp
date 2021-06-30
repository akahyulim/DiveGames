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
		auto matrixLocalPos = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&GetLocalPosition()));
		
	}

	DirectX::XMFLOAT3 Transform::GetPosition()
	{
		return DirectX::XMFLOAT3();
	}

	void Transform::SetPosition(const DirectX::XMFLOAT3& position)
	{
		if (DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&GetPosition()), DirectX::XMLoadFloat3(&position)))
			return;

		// ��Ʈ���� �׳� ���� ���� �� ����
		// �θ� �ִٸ� ��ȯ ��� ������� ���� ���� ����
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(DirectX::XMFLOAT3(x, y, z));
	}
	
	void Transform::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		if (DirectX::XMVector3Equal(DirectX::XMLoadFloat3(&GetLocalPosition()), DirectX::XMLoadFloat3(&position)))
			return;

		mLocalPosition = position;

		UpdateTransform();
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(DirectX::XMFLOAT3(x, y, z));
	}

	DirectX::XMFLOAT4 Transform::GetRotation()
	{

	}

	void Transform::SetRotation(const DirectX::XMFLOAT3& rotation)
	{

	}

	void Transform::SetLocalRotation(const DirectX::XMFLOAT3& rotation)
	{

	}

	DirectX::XMFLOAT3 Transform::GetScale()
	{

	}

	void Transform::SetScale(const DirectX::XMFLOAT3& scale)
	{

	}

	void Transform::SetLocalScale(const DirectX::XMFLOAT3& scale)
	{

	}
}