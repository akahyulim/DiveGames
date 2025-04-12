#pragma once
#include "core/UUID.h"
#include <DirectXMath.h>

namespace Dive
{
	struct IDComponent
	{
		UUID ID = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct ActiveComponent
	{
		bool IsActive;

		ActiveComponent() : IsActive(true) {}
		ActiveComponent(const ActiveComponent&) = default;
		ActiveComponent(bool active) : IsActive(active) {}
	};

	struct TransformComponent
	{
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT4 Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const DirectX::XMFLOAT3& pos) : Position(pos) {}
	};

	struct CameraComponent
	{

	};
}