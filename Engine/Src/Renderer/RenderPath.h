#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;

	struct RenderTargetInfo
	{
		DXGI_FORMAT format;
		std::string name;
		DirectX::XMFLOAT2 size;
	};

	enum class eRenderCommandType
	{
		None = 0,
		Clear,
		ScenePass,
	};

	struct RenderPathCommand
	{
		// 파일을 직접 읽어들여 추가 설정을 하기도 한다.

		eRenderCommandType type;

		std::string vertexShaderName;
		std::string pixelShaderName;

		DirectX::XMFLOAT4 clearColor;
		float clearDepth;
		unsigned int clearStencil;
	};

	class RenderPath : public Resource
	{
		DIVE_OBJECT(RenderPath, Resource)

	public:
		explicit RenderPath(Context* pContext);
		~RenderPath() override;

		bool LoadFromFile(const std::string& filepath) override;
		bool SaveToFile(const std::string& filepath) override;

		void AddCommand(const RenderPathCommand& command);
		unsigned int GetNumCommands() const { return static_cast<unsigned int>(m_RenderPathCommands.size()); }

	public:
		std::vector<RenderTargetInfo> m_RenderTargetInfos;
		std::vector<RenderPathCommand> m_RenderPathCommands;
	};
}