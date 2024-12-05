#pragma once

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eRenderPathType : uint8_t
	{
		Forward = 0,
		Deferred,
		Max
	};

	enum class eRenderTargetType : uint8_t
	{
		GBuffer_DepthStencil = 0,
		GBuffer_Diffuse,
		GBuffer_Normal,
		GBuffer_Specular,
		FrameRender,
		Max
	};

	//-------------------------------------------------------------------------------------
	// Ŭ���� ���� ����
	//-------------------------------------------------------------------------------------
	class Graphics;
	class Texture2D;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		bool Initialize(uint32_t width, uint32_t height, std::shared_ptr<Graphics> pGraphics);
		void Shutdown();

		void Tick();

		void ResizeTargets(uint32_t width, uint32_t height);

		uint32_t GetTargetWidth() const { return m_Width; }
		uint32_t GetTargetHeight() const { return m_Height; }

		uint64_t GetFrameCount() const { return m_FrameCount; }

		ID3D11RenderTargetView* GetRenderTargetView(eRenderTargetType type);
		ID3D11ShaderResourceView* GetShaderResourceView(eRenderTargetType type);
		// GetDepthStencilView�� ������� array�� enum class�� ������ �����ؾ� �� �� ����.

	private:
	private:
		std::shared_ptr<Graphics> m_pGraphics;

		uint32_t m_Width;
		uint32_t m_Height;

		uint64_t m_FrameCount;

		std::array<std::unique_ptr<Texture2D>, static_cast<size_t>(eRenderTargetType::Max)> m_RenderTargets;
	};
}