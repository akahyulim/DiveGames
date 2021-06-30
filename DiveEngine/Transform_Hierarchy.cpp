#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"

namespace dive
{
	Transform* Transform::GetRoot()
	{
		if (!mParent)
			return this;
		else
			return mParent->GetRoot();
	}

	// =============================================//
	// 상위 노드의 부모를 하위 노드로 변경할 경우	//
	// 모두 사라지는 버그가 있다.					//
	//==============================================//
	void Transform::SetParent(Transform* parentTransform)
	{
		if (!parentTransform)
		{
			BecomeOrphan();
			return;
		}

		if (this->GetID() == parentTransform->GetID())
			return;

		if (mParent)
		{
			for (auto it = mParent->mChildren.begin(); it != mParent->mChildren.end();)
			{
				if ((*it)->GetID() == this->GetID())
				{
					it = mParent->mChildren.erase(it);
				}
				else
					++it;
			}
		}

		parentTransform->mChildren.emplace_back(this);
		mParent = parentTransform;
	}

	void Transform::BecomeOrphan()
	{
		if (!mParent)
			return;

		auto pOldParent = mParent;
		mParent = nullptr;

		// UpdateTransform();

		pOldParent->AcquireChidren();
	}

	Transform* Transform::Find(const std::string& name) const
	{
		if (!mChildren.empty())
		{
			for (auto& child : mChildren)
			{
				if (child->GetOwner() && child->GetOwner()->GetName() == name)
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

	bool Transform::IsChildOf(const Transform* parentTransform) const
	{
		if (!parentTransform)
			return false;

		// parentTransform이 자신이라도 ture를 리턴한다.
		if (parentTransform->GetID() == GetID())
			return true;

		if (mParent->GetID() == parentTransform->GetID())
			return true;

		return mParent->IsChildOf(parentTransform);
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
	}

	void Transform::AcquireChidren()
	{
		mChildren.clear();
		mChildren.shrink_to_fit();

		auto allGameObjects = Scene::GetGlobalScene().GetAllGameObjects();

		for (auto gameObject : allGameObjects)
		{
			if (gameObject->GetTransform()->HasParent())
			{
				if (gameObject->GetTransform()->GetParent()->GetID() == GetID())
				{
					mChildren.emplace_back(gameObject->GetTransform());
					gameObject->GetTransform()->AcquireChidren();
				}
			}
		}
	}
}