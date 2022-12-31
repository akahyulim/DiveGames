#pragma once
#include "Resource/Resource.h"

namespace Dive
{
	class Context;

	// ť��� ���̽�
	// GraphicsDefs�� �ű���
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

	// ���� Ÿ�� ���� ����ü.
	struct RenderTargetInfo
	{
		// Load �Լ��� ������.

		DXGI_FORMAT Format;
		std::string Name;
		DirectX::XMFLOAT2 Size;

		// �߰� ������ ������.
	};

	// Ŀ�ǵ� Ÿ�� ������.
	enum class eRenderCommandType
	{
		None = 0,
		Clear,
		ScenePass,
		ForwardLights,
	};

	// ������ ����� ����� ����ü.
	struct RenderPathCommand
	{
		// ���� Load �Լ��� ���� ������.

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

	// ���� Ÿ�ٰ� Ŀ�ǵ带 �����Ͽ� ���� �н��� �����ϴ� Ŭ����.
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

		// urho�� public�̶� ���� ������ �����ϴ�.
	private:
		std::vector<RenderTargetInfo> m_RenderTargets;
		std::vector<RenderPathCommand> m_Commands;
	};
}