#include "stdafx.h"
#include "WorldSerializer.h"
#include "World.h"
#include "GameObject.h"
#include "Components/Transform.h"

#include <dive.h>

#include <yaml-cpp/yaml.h>

namespace Dive
{
	static void SerializeGameObject(YAML::Emitter& out, GameObject gameObject)
	{
		out << YAML::BeginMap;

		// uuid�� ����
		// component���� if(gameObject.HasComponent)�� ����ȭ

		out << YAML::EndMap;
	}

	WorldSerializer::WorldSerializer(std::shared_ptr<World> pWorld)
		: m_pWorld(pWorld)
	{
	}

	void WorldSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "World" << YAML::Value << path.filename().c_str();

		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool WorldSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch(YAML::ParserException e)
		{
			DV_LOG(WorldSerializer, err, "���� ����({:s})�� �ε忡 �����Ͽ����ϴ�.", path.string());
			return false;
		}

		if (!data["World"])
			return false;

		std::string worldName = data["World"].as<std::string>();
		DV_LOG(WorldSerializer, trace, "{:s} �ε� ����", worldName);


		return true;
	}
}