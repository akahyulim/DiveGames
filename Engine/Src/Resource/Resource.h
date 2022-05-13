#pragma once
#include "Base/Object.h"

namespace Dive
{
	static const char* EXTENSION_MODEL		= "model";
	static const char* EXTENSION_TEXTURE	= "texture";
	static const char* EXTENSION_MATERIAL	= "mtrl";

	enum class eResourceType
	{
		Texture,
		Texture2D,
		Model,
		Material,
		Count
	};

#define RESOURCE_CLASS_TYPE(type)	static eResourceType GetStaticType() { return eResourceType::type;} \
									virtual eResourceType GetType() const override { return GetStaticType(); } \
									virtual const char* GetTypeStr() const override { return #type;} 
 
	class Resource : public Object
	{
	public:
		Resource(const std::string& name = "", unsigned long long id = 0);
		virtual ~Resource() = default;

		virtual bool SaveToFile(const std::string& filepath) { return true; }
		virtual bool LoadFromFile(const std::string& filepath)	{ return true; }

		virtual eResourceType GetType()		const = 0;
		virtual const char* GetTypeStr()	const = 0;

		const std::string GetExportedFilepath() const { return m_ExportedFilepath; }
		const std::string GetEngineFilepath() const { return m_EngineFilepath; }

		// 현재 Model의 경우 Save, Load에서 하드 코딩 해놓았다.
		void SetFilepath(const std::string& filepath)
		{
			// 원본 포멧일 경우
			// 엔진 포멧 경로를 생성한 후 저장한다.
			// 이때 원본 포멧별로 엔진 포멧을 만드는 함수가 필요하다.

			// 엔진 포멧일 경우
			// 원본 포멧 경로는 없으므로 그냥 clear한다.

			// 리소스의 이름은 이 곳에서 자동으로 설정된다.
			
			// 타입별 폴더 경로를 저장한다.
			// 상대경로에서 뽑아내는 것 같다.
		}
		
	protected:
		std::string m_ExportedFilepath;
		std::string m_EngineFilepath;
	};
}