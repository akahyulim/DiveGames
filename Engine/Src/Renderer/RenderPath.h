#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;

	// 큐브맵 페이스
	// GraphicsDefs로 옮기자
	enum eCubeMapFace
	{
		PositiveX,
		NegativeX,
		PositiveY,
		NegativeY,
		PositiveZ,
		NegativeZ,
		MaxFaces
	};

	// 렌더 타겟 정의 구조체.
	struct RenderTargetInfo
	{
		// Load 함수를 가진다.

		DXGI_FORMAT Format;
		std::string Name;
		DirectX::XMFLOAT2 Size;

		// 추가 정보를 가진다.
	};

	// 커맨드 타입 열거자.
	enum class eRenderCommandType
	{
		None = 0,
		Clear,
		ScenePass,
		ForwardLights,
	};

	// 렌더링 방법을 기술한 구조체.
	struct RenderPathCommand
	{
		// 역시 Load 함수를 직접 가진다.

		std::string Tag;
		eRenderCommandType Type;
		std::string Pass;
		uint32_t PassIndex;

		std::string VertexShaderName;
		std::string PixelShaderName;

		std::string VertexShaderDefines;
		std::string PixelShaderDefines;

		std::string TextureNames[16];

		std::vector<std::pair<std::string, eCubeMapFace>> Outputs;
		std::string DepthStencilName;

		DirectX::XMFLOAT4 ClearColor;
		float ClearDepth;
		uint32_t ClearStencil;

		bool bVertexLights;

		// sort mode
	};

	// 렌더 타겟과 커맨드를 관리하여 렌더 패스를 구성하는 클래스.
	class RenderPath : public Resource
	{
		DIVE_OBJECT(RenderPath, Resource)

	public:
		explicit RenderPath(Context* pContext);
		~RenderPath() override;

		bool LoadFromFile(const std::string& filepath) override;
		bool SaveToFile(const std::string& filepath) override;

		void SetRenderTarget(uint32_t index, const RenderTargetInfo& info);
		void AddRenderTarget(const RenderTargetInfo& info);
		void RemoveRenderTarget(uint32_t index);

		RenderPathCommand* GetCommand(uint32_t index);
		void SetCommand(uint32_t index, const RenderPathCommand& command);
		void InsertCommand(uint32_t index, const RenderPathCommand& command);
		void AddCommand(const RenderPathCommand& command);
		void RemoveCommand(uint32_t index);

		uint32_t GetRendertargetCount() const { return static_cast<uint32_t>(m_RenderTargets.size()); }
		uint32_t GetCommandCount() const { return static_cast<uint32_t>(m_Commands.size()); }

		const std::vector<RenderTargetInfo>& GetRenderTargets() const { return m_RenderTargets; }
		const std::vector<RenderPathCommand>& GetCommands() const { return m_Commands; }

		// urho는 public이라 직접 접근이 가능하다.
	private:
		std::vector<RenderTargetInfo> m_RenderTargets;
		std::vector<RenderPathCommand> m_Commands;
	};
}