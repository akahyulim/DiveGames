#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"

namespace dive
{
	//==================================================//
	// ���� nullptr�� ��ȯ���� �ʽ��ϴ�.				//
	// �θ� �������� ���� ��� �ڱ��ڽ��� ��ȯ�մϴ�.	//
	//==================================================//
	Transform* Transform::GetRoot()
	{
		if (!mParent)
			return this;
		else
			return mParent->GetRoot();
	}

	// =============================================//
	// ���� ����� �θ� ���� ���� ������ ���	//
	// ��� ������� ���װ� �ִ�.					//
	//==============================================//
	void Transform::SetParent(Transform* parent)
	{
		if (!parent)
		{
			BecomeOrphan();
			return;
		}

		if (this->GetInstanceID() == parent->GetInstanceID())
			return;

		if (mParent)
		{
			for (auto it = mParent->mChildren.begin(); it != mParent->mChildren.end();)
			{
				if ((*it)->GetInstanceID() == this->GetInstanceID())
				{
					it = mParent->mChildren.erase(it);
				}
				else
					++it;
			}
		}

		parent->mChildren.emplace_back(this);
		mParent = parent;
	}

	void Transform::BecomeOrphan()
	{
		if (!mParent)
			return;

		auto oldParent = mParent;
		mParent = nullptr;

		UpdateTransform();

		// �� ����Լ��� ���ſ� ���̴µ�
		// ���� �̷��� �� �ʿ䰡 �ֳ�?
		// �׳� �θ��� children���� �ڽŸ� ����� �Ǵµ�..
		// �׷����� removeChild������ ����.
		oldParent->AcquireChidren();
	}

	//==================================//
	// GameObject�� �̸��� ����մϴ�.	//
	//==================================//
	Transform* Transform::Find(const std::string& name) const
	{
		if (!mChildren.empty())
		{
			for (auto child : mChildren)
			{
				assert(child->GetGameObject());

				if (child->GetGameObject()->GetName() == name)
				{
					return child;
				}
			}
		}

		return nullptr;
	}

	Transform* Transform::GetChild(unsigned int index) const
	{
		if (mChildren.empty() || mChildren.size() - 1 < index)
			return nullptr;

		return mChildren[index];
	}

	bool Transform::IsChildOf(const Transform* parent) const
	{
		if (!parent)
			return false;

		// parent�� �ڽ��̶� ture�� �����Ѵ�.
		if (parent->GetInstanceID() == GetInstanceID())
			return true;

		if (mParent->GetInstanceID() == parent->GetInstanceID())
			return true;

		return mParent->IsChildOf(parent);
	}

	void Transform::DetachChildren()
	{
		if (mChildren.empty())
			return;

		for (auto pChild : mChildren)
		{
			pChild->mParent = nullptr;
		}

		mChildren.clear();
		mChildren.shrink_to_fit();

		UpdateTransform();
	}

	// ���� �Ӹ��� �ȵ��� ���µ�
	// ���������� �����ϴ� �Լ����
	// UpdateTransform�� �ʿ��ϴ�.
	void Transform::AcquireChidren()
	{
		mChildren.clear();
		mChildren.shrink_to_fit();

		auto& allGameObjects = Scene::GetGlobalScene().GetAllGameObjects();

		for (auto gameObject : allGameObjects)
		{
			if (gameObject->GetTransform()->HasParent())
			{
				if (gameObject->GetTransform()->GetParent()->GetInstanceID() == GetInstanceID())
				{
					mChildren.emplace_back(gameObject->GetTransform());
					gameObject->GetTransform()->AcquireChidren();
				}
			}
		}
	}
}