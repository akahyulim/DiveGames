#pragma once
#include "Component.h"

namespace Dive
{
	class Context;
	class GameObject;
	class FileStream;


	class Transform : public Component
	{
		DIVE_OBJECT(Transform, Component);

	public:
		explicit Transform(Context* context, GameObject* owner);
		~Transform();

		// serialization
		void Serialize(FileStream& stream) override;
		void Deserialize(FileStream& stream) override;

		void Copy(Component* other) override;

		//= position
		DirectX::XMVECTOR GetPosition();
		DirectX::XMVECTOR GetLocalPosition();
		void SetPosition(const DirectX::XMVECTOR& position);
		void SetLocalPosition(const DirectX::XMVECTOR& position);
		void SetPosition(float x, float y, float z)			{ SetPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }
		void SetLocalPosition(float x, float y, float z)	{ SetLocalPosition(DirectX::XMVectorSet(x, y, z, 1.0f)); }

		//= rotation
		DirectX::XMVECTOR GetRotation();
		DirectX::XMVECTOR GetRotationDegrees();
		DirectX::XMVECTOR GetRotationRadians();
		DirectX::XMVECTOR GetLocalRotation();
		DirectX::XMVECTOR GetLocalRotationDegrees();
		DirectX::XMVECTOR GetLocalRotationRadians();
		void SetRotation(const DirectX::XMVECTOR& quaternion);
		void SetLocalRotation(const DirectX::XMVECTOR& quaternion);
		void SetRotationDegrees(const DirectX::XMVECTOR& degrees);
		void SetLocalRotationDegrees(const DirectX::XMVECTOR& degrees);
		void SetRotationRadians(const DirectX::XMVECTOR& radians);
		void SetLocalRotationRadians(const DirectX::XMVECTOR& radians);

		//= Scale
		DirectX::XMVECTOR GetScale();
		DirectX::XMVECTOR GetLocalScale();
		void SetScale(const DirectX::XMVECTOR& scale);
		void SetLocalScale(const DirectX::XMVECTOR& scale);
		void SetScale(float x, float y, float z)			{ SetScale(DirectX::XMVectorSet(x, y, z, 1.0f)); }
		void SetLocalScale(float x, float y, float z)		{ SetLocalScale(DirectX::XMVectorSet(x, y, z, 1.0f)); }

		//= Move
		void Translate(const DirectX::XMVECTOR& delta);
		void Rotate(const DirectX::XMVECTOR& delta);

		//= etc
		DirectX::XMMATRIX GetMatrix()		const { return DirectX::XMLoadFloat4x4(&m_matrix); }
		DirectX::XMMATRIX GetLocalMatrix()	const { return DirectX::XMLoadFloat4x4(&m_localMatrix); }

		DirectX::XMVECTOR GetUp();
		DirectX::XMVECTOR GetDown();
		DirectX::XMVECTOR GetLeft();
		DirectX::XMVECTOR GetRight();
		DirectX::XMVECTOR GetBackward();
		DirectX::XMVECTOR GetForward();


		//= hierarchy
		bool IsRoot()										const { return !HasParent(); }
		bool HasParent()									const { return m_parent; }
		void SetParent(Transform* newParent);
		void BecomeOrphan();
		bool IsAncestor(const Transform* target)			const;
		bool IsDescendant(const Transform* target)			const;
		void GetDescendants(std::vector<Transform*>* descendants);
		bool HasChildren()									const { return !m_children.empty() ? true : false; }
		size_t GetChildrenCount()							const { return m_children.size(); }
		void AddChild(Transform* child);
		Transform* GetRoot();
		Transform* GetParent()								const { return m_parent; }
		Transform* GetChildByName(const std::string& name)	const;
		Transform* GetChildByIndex(unsigned int index)		const;
		const std::vector<Transform*>& GetChildren()		const { return m_children; }
		void AcqurieChildren();


	private:
		Transform(const Transform&)				= delete;
		Transform& operator=(const Transform&)	= delete;

		void updateTransform();

	private:
		DirectX::XMFLOAT3 m_localPosition;
		DirectX::XMFLOAT4 m_localRotation;
		DirectX::XMFLOAT3 m_localScale;

		DirectX::XMFLOAT4X4 m_matrix;
		DirectX::XMFLOAT4X4 m_localMatrix;

		Transform* m_parent;
		std::vector<Transform*> m_children;
	};
}