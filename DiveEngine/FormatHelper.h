/*
	- Sandbox가 mSupportedShaderFormats를 참조할 수 없다고 에러를 내뿜어 관련 코드를 모두 주석화했다.
	- 위치가 마음에 들지 않는다. 다시 core로 복귀?

	제작: 서보윤( akahyulim@gmail.com )
*/
#pragma once

// temp includes
#include <vector>
#include <string>


// DIVE ENGINE EXTENSION
static const char* DEE_SCENE		= ".scene";
static const char* DEE_MATERIAL		= ".mat";
static const char* DEE_TEXTURE		= ".texture";
static const char* DEE_MESH			= ".mesh";
static const char* DEE_MODEL		= ".model";

namespace dive
{
	// 임포터를 에디터에 넣을꺼면
	// 엔진 확장자 확인만 하는 함수들을 helper로 묶는 편이 나을 것 같다.
	class FormatHelper
	{
	public:
		FormatHelper() = default;
		~FormatHelper() = default;

		static void Initialize();

		// 지원 확장자 비교 : 포멧을 문자열로 전달하면 비교 후 bool 리턴
		// 즉, 각종 임포터가 지원하는 확장자를 확인하는 함수들이다.
		// 이미지
		static bool IsSupportedImageFile(const std::string& filepath);
		// 모델
		// 스크립트
		static bool IsSupportedShaderFile(const std::string& path);
		// 폰트
		// 이하 엔진 지원 포멧 : 구분 이해가 필요하다.
		static bool IsEngineSceneFile(const std::string& path);
		static bool IsEngineMaterialFile(const std::string& path);
		static bool IsEngineTextureFile(const std::string& path);
		static bool IsEngineMeshFile(const std::string& path);
		static bool IsEngineModelFile(const std::string& path);
		// 프리팹
		// 메타 데이터
		// 오디오

		// 이것들도 결국 임포터가 지원하는 포멧들을 리턴하는 함수다.
		// 즉, 임포터를 에디터에 넣을거면 옮겨야 한다.
		static std::vector<std::string> GetSupportedImageFormats()		{ return mSupportedImageFormats; }
		static std::vector<std::string> GetSupportedModelFormats()		{ return mSupportedModelFormats; }
		static std::vector<std::string> GetSupportedScriptFormats()		{ return mSupportedScriptFormats; }
		static std::vector<std::string> GetSupportedSahderFormats()		{ return mSupportedShaderFormats; }
		static std::vector<std::string> GetSupportedFontFormats()		{ return mSupportedFontFormats; }

	private:
		static std::vector<std::string> mSupportedImageFormats;
		static std::vector<std::string> mSupportedModelFormats;
		static std::vector<std::string> mSupportedScriptFormats;
		static std::vector<std::string> mSupportedShaderFormats;
		static std::vector<std::string> mSupportedFontFormats;
		// 오디오
		// 실제 에셋은 material, scene 등 더 많지만 위의 것들만 관리했다.
		// 아마 material과 scene등은 단일 포멧이기 때문일 것이다.
	};
}
