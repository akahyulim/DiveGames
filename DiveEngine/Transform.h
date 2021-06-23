#pragma once
#include "Component.h"
#include "GraphicsInclude.h"
#include <string>
#include <vector>

namespace dive
{
	class FileStream;

	class Transform : public Component
	{
	public:
		Transform();
		~Transform();

		// �������̵� �Լ�
		void Update(float deltaTime) override;
		void Serialize(FileStream* fileStream) override;
		void Deserialize(FileStream* fileStream) override;

		// ��ȯ �Լ�

		// �������� �Լ� ===================================================================================
		// ������ �����Ѵ�.
		Transform* GetRoot();
		// �θ� ���� ���θ� �����Ѵ�.
		bool HasParent() const { return (mParent != nullptr); }
		// �θ� �����͸� �����Ѵ�.
		Transform* GetParent() const { return mParent; }
		void SetParent(Transform* parentTransform);
		// �θ� �����Ѵٸ� �����Ѵ�.
		void BecomeOrphan();
		// �ڽ��� ���� �����Ѵ�.
		unsigned int GetChildCount() const { return static_cast<unsigned int>(mChildren.size()); }
		// �̸����� �ڽ��� ã�� �����Ѵ�.
		Transform* Find(const std::string& name) const;
		// �ε����� �ڽ��� ã�� �����Ѵ�.
		Transform* GetChild(unsigned int index) const;
		// ��� �ڽ��� �����Ѵ�.
		std::vector<Transform*> GetChildren() { return mChildren; }
		// �ڽ��� parentTransform�� �ڽ����� Ȯ���Ѵ�.
		bool IsChildOf(const Transform* parentTransform) const;
		// ��� �ڽ��� �����. �ڽĵ��� �������� �ʰ� �θ� ���ְ� ���� �� ����Ѵ�.
		void DetachChildren();
		// �ڽĵ�κ��� �θ� �ڽ� ���踦 ��������� �籸���Ѵ�.
		void AcquireChidren();

	private:

	private:
		// transform

		// heirarchy
		Transform* mParent = nullptr;
		std::vector<Transform*> mChildren;
	};
}