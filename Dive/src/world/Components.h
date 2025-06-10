#pragma once
#include "core/InstanceID.h"
#include "graphics/RenderTexture.h"
#include "rendering/Mesh.h"
#include "rendering/Material.h"

#include <DirectXMath.h>
#include <entt/entt.hpp>

namespace Dive
{
	struct IDComponent
	{
		InstanceID ID = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct NameComponent
	{
		std::string Name;

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(const std::string& tag) : Name(tag) {}
	};

	struct TagComponent
	{
		enum class eTag
		{
			Untagged,
			EditorOnly,
			MainCamera,
			Player,
			Enemy,
			Wall,
			Item,
			Respawn,
			Finish
		};

		eTag Tag = eTag::Untagged;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(eTag tag) : Tag(tag) {}
	};

	struct ActiveComponent
	{
		bool IsActive = true;

		ActiveComponent() = default;
		ActiveComponent(const ActiveComponent&) = default;
		ActiveComponent(bool active) : IsActive(active) {}
	};

	struct LocalTransform
	{
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 Scale = { 1.0f, 1.0f, 1.0f };

		void FromMatrix(const DirectX::XMFLOAT4X4& matrix);

		void Translate(const DirectX::XMFLOAT3& translation);

		void Rotate(const DirectX::XMFLOAT4& quaternion);
		void RotateX(float angle);
		void RotateY(float angle);
		void RotateZ(float angle);

		DirectX::XMFLOAT3 Forward() const;
		DirectX::XMFLOAT3 Up() const;
		DirectX::XMFLOAT3 Right() const;
	};

	struct LocalToWorld
	{
		DirectX::XMFLOAT4X4 Value = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	};

	struct ParentComponent
	{
		entt::entity Parent = entt::null;
	};

	struct Children
	{
		std::vector<entt::entity> entities;
	};

	struct RenderMesh
	{
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
	};

	struct CameraComponent
	{
		enum class eProjectionType { Perspective, Orthographic };
		eProjectionType Type = eProjectionType::Perspective;

		float NearClip = 0.1f;
		float FarClip = 1000.0f;
		float FileOfView = 45.0f;
		float AspectRatio = 0.0f;
		float OrthographicSize = 0.0f;

		DirectX::XMFLOAT4X4 ProjectionMatrix{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		// x, y, width, height를 0.0 ~ 1.0 사이의 비율로 관리
		DirectX::XMFLOAT4 Viewport{ 0.0f, 0.0f, 1.0f, 1.0f };

		RenderTexture* RenderTarget = nullptr;
		DirectX::XMFLOAT4 ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}