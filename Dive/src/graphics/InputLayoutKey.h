#pragma once
#include <functional>
#include "Shader.h"

namespace Dive
{
	struct InputLayoutKey
	{
		InputLayoutKey() = default;
		InputLayoutKey(const std::string name, eInputLayout type)
			: shaderName(name)
			, layoutType(type)
		{
		}

		std::string shaderName;
		eInputLayout layoutType;

		bool operator==(const InputLayoutKey& other) const
		{
			return shaderName == other.shaderName && layoutType == other.layoutType;
		}
	};
}

namespace std
{
	template <>
	struct hash<Dive::InputLayoutKey> {
		size_t operator()(const Dive::InputLayoutKey& key) const {
			size_t h1 = hash<std::string>{}(key.shaderName);
			size_t h2 = hash<int>{}(static_cast<int>(key.layoutType));
			return h1 ^ (h2 << 1);
		}
	};
}
