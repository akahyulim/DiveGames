#pragma once
#include "core/UUID.h"
#include "graphics/RenderTexture.h"
#include <DirectXMath.h>

namespace Dive
{
	struct IDComponent
	{
		UUID ID = 0;

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

	struct TagComponent
	{
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

	struct TransformComponent
	{
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent* Parent = nullptr;
		DirectX::XMFLOAT4X4 WorldMatrix{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		bool Dirty = true;

		DirectX::XMFLOAT4X4 GetLocalMatrix() const
		{
			auto localMat = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&Scale))
				* DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&Rotation))
				* DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&Position));

			DirectX::XMFLOAT4X4 transform{};
			DirectX::XMStoreFloat4x4(&transform, localMat);

			return transform;
		}

		DirectX::XMFLOAT4X4 GetWorldMatrix() const
		{
			DirectX::XMFLOAT4X4 localMat = GetLocalMatrix();

			if (Parent)
			{
				DirectX::XMFLOAT4X4 transform{};
				auto parentWorldMat = Parent->GetWorldMatrix();
				DirectX::XMStoreFloat4x4(&transform,
					DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&parentWorldMat), DirectX::XMLoadFloat4x4(&localMat)));

				return transform;
			}
			return localMat;
		}

		void MarkDirty() { Dirty = true; }

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const DirectX::XMFLOAT3& pos) : Position(pos) {}
	};

	struct CameraComponent
	{
		enum class eProjectionType { Perspective, Orthographic };
		eProjectionType Type = eProjectionType::Perspective;

		float FileOfView = 45.0f;
		float NearClip = 0.1f;
		float FarClip = 1000.0f;
		float AspectRatio = 0.0f;
		float OrthographicSize = 0.0f;

		DirectX::XMFLOAT4X4 ProjectionMatrix{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		RenderTexture* RenderTarget = nullptr;
		DirectX::XMFLOAT4 ClearColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};
}