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

		// uuid로 구분
		// component들은 if(gameObject.HasComponent)로 직렬화

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
			DV_LOG(WorldSerializer, err, "월드 파일({:s})을 로드에 실패하였습니다.", path.string());
			return false;
		}

		if (!data["World"])
			return false;

		std::string worldName = data["World"].as<std::string>();
		DV_LOG(WorldSerializer, trace, "{:s} 로드 성공", worldName);


		return true;
	}
}