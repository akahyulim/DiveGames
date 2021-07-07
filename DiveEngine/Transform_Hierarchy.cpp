#include "Transform.h"
#include "Scene.h"
#include "GameObject.h"

namespace dive
{
	//==================================================//
	// 절대 nullptr을 반환하지 않습니다.				//
	// 부모가 존재하지 않을 경우 자기자신을 반환합니다.	//
	//==================================================//
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

		// 이 재귀함수는 무거워 보이는데
		// 굳이 이렇게 할 필요가 있나?
		// 그냥 부모의 children에서 자신만 지우면 되는데..
		// 그러고보니 removeChild같은게 없네.
		oldParent->AcquireChidren();
	}

	//==================================//
	// GameObject의 이름을 사용합니다.	//
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

		// parent가 자신이라도 ture를 리턴한다.
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

	// 현재 머리가 안돌아 가는데
	// 계층구조를 변경하는 함수라면
	// UpdateTransform이 필요하다.
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