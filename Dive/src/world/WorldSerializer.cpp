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

	WorldSerializer::WorldSerializer(World* pWorld)
		: m_pWorld(pWorld)
	{
	}

	void WorldSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "World" << YAML::Value << m_pWorld->m_Name;

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
		catch (YAML::ParserException e)
		{
			DV_LOG(WorldSerializer, err, "���� ����({:s})�� ������ȭ�� �����Ͽ����ϴ�.", path.string());
			return false;
		}

		if (!data["World"])
			return false;

		m_pWorld->m_Name = data["World"].as<std::string>();

		return true;
	}
}