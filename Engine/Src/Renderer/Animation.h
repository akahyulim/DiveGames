#pragma once
#include "Resource/Resource.h"
#include "Bone.h"

namespace Dive
{
	class Animation : public Resource
	{
	public:
		Animation();
		virtual ~Animation();

		bool LoadFromFile(const std::string& filePath) override;
		bool SaveToFile(const std::string& filePath) override;

		void InsertBone(const Bone& bone) { m_Bones.emplace_back(bone); }
		Bone* FindBone(const std::string& name);

		float GetDuration() const { return m_Duration; }
		void SetDuration(float time) { m_Duration = time; }

		float GetTickPerSecond() const { return m_TickPerSecond; }
		void SetTickPerSecond(float time) { m_TickPerSecond = time; }

	private:
	private:
		float m_Duration;
		float m_TickPerSecond;
		std::vector<Bone> m_Bones;
		// boneInfoMap(결국 동일한 데이터를 Model과 Animation 둘 다 가지게 된다.)
	};
}