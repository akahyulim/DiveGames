#pragma once
#include "Resource/Resource.h"
#include "Bone.h"
#include "Model.h"

#include "Scene/GameObject.h"

namespace Dive
{
	class Model;

	class Animation : public Resource
	{
	public:
		Animation();
		Animation(const std::string& name, float duration, float tickPerSecond, uint32_t numChannels);
		virtual ~Animation();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void InsertBone(const Bone& bone) { m_Bones.emplace_back(bone); }

		Bone* FindBone(const std::string& name);

		float GetDuration() const { return m_Duration; }
		void SetDuration(float duration) { m_Duration = duration; }
		
		float GetTickPerSecond() const { return m_TickPerSecond; }
		void SetTickPerSecond(float tick) { m_TickPerSecond = tick; }

		uint32_t GetNumChannel() const { return m_NumChannel; }
		void SetNumChannel(uint32_t size) { m_NumChannel = size; }

		uint32_t GetNumBone() const { static_cast<uint32_t>(m_Bones.size()); }

		const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }
		void SetBoneIDMap(const std::unordered_map<std::string, BoneInfo>& boneInfoMap) { m_BoneInfoMap = boneInfoMap; }

		GameObject* GetRootGameObject() { return m_pRootGameObject; }
		void SetRootGameObject(GameObject* pGameObject) { m_pRootGameObject = pGameObject; }

	private:
	private:
		float m_Duration;		
		float m_TickPerSecond;	
		uint32_t m_NumChannel;

		std::vector<Bone> m_Bones;

		std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

		GameObject* m_pRootGameObject;
	};
}