#pragma once

namespace Dive
{
	class Scene;

	class Serializer
	{
	public:
		Serializer(Scene* pScene);
		~Serializer() = default;

		void Serialize(const std::string& dir);
		bool Deserialize(const std::string& filepath);

	private:
		Scene* m_pScene = nullptr;
	};
}