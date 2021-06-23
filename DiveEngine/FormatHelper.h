/*
	- Sandbox�� mSupportedShaderFormats�� ������ �� ���ٰ� ������ ���վ� ���� �ڵ带 ��� �ּ�ȭ�ߴ�.
	- ��ġ�� ������ ���� �ʴ´�. �ٽ� core�� ����?

	����: ������( akahyulim@gmail.com )
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
	// �����͸� �����Ϳ� ��������
	// ���� Ȯ���� Ȯ�θ� �ϴ� �Լ����� helper�� ���� ���� ���� �� ����.
	class FormatHelper
	{
	public:
		FormatHelper() = default;
		~FormatHelper() = default;

		static void Initialize();

		// ���� Ȯ���� �� : ������ ���ڿ��� �����ϸ� �� �� bool ����
		// ��, ���� �����Ͱ� �����ϴ� Ȯ���ڸ� Ȯ���ϴ� �Լ����̴�.
		// �̹���
		static bool IsSupportedImageFile(const std::string& filepath);
		// ��
		// ��ũ��Ʈ
		static bool IsSupportedShaderFile(const std::string& path);
		// ��Ʈ
		// ���� ���� ���� ���� : ���� ���ذ� �ʿ��ϴ�.
		static bool IsEngineSceneFile(const std::string& path);
		static bool IsEngineMaterialFile(const std::string& path);
		static bool IsEngineTextureFile(const std::string& path);
		static bool IsEngineMeshFile(const std::string& path);
		static bool IsEngineModelFile(const std::string& path);
		// ������
		// ��Ÿ ������
		// �����

		// �̰͵鵵 �ᱹ �����Ͱ� �����ϴ� ������� �����ϴ� �Լ���.
		// ��, �����͸� �����Ϳ� �����Ÿ� �Űܾ� �Ѵ�.
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
		// �����
		// ���� ������ material, scene �� �� ������ ���� �͵鸸 �����ߴ�.
		// �Ƹ� material�� scene���� ���� �����̱� ������ ���̴�.
	};
}
