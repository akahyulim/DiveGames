#pragma once
#include "resource/Resource.h"
#include "graphics/Graphics.h"
#include "graphics/ConstantBuffer.h"
#include "graphics/ShaderProgram.h"

namespace Dive
{
	class Texture;
	class Texture2D;

	enum class eMapType : uint8_t
	{
		Diffuse,
		Normal,
		Count
	};

	enum class eRenderingMode
	{
		Opqaue,
		Transparent
	};

	struct alignas(16) MaterialData
	{
		DirectX::XMFLOAT4 diffuseColor{0.0f, 0.0f, 0.0f, 1.0f};
		DirectX::XMFLOAT2 tiling{1.0f, 1.0f};
		DirectX::XMFLOAT2 offset{0.0f, 0.0f};
		uint32_t textureMask{};
		uint32_t padding[3]{};
	};

	// 유니티의 경우 파생 클래스로 나뉘지 않는다.
	// Shader에 따라 Inspector에서 제어하는 데이터들이 다를 뿐이다.
	class Material : public Resource
	{
	public:
		Material();
		~Material() override = default;

		bool LoadFromFile(const std::filesystem::path& filepath) override;
		bool SaveToFile(const std::filesystem::path& filepath) override;

		void Bind();

		std::shared_ptr<Texture2D> GetMap(eMapType type);
		void SetMap(eMapType type, std::shared_ptr<Texture2D> texture);
		void SetMap(eMapType type, const std::string& textureName);
		void SetMap(eMapType type, const std::filesystem::path& filepath);

		DirectX::XMFLOAT4 GetDiffuseColor() const { return m_cpuBuffer.diffuseColor; }
		void SetDiffuseColor(const DirectX::XMFLOAT4& color) { m_cpuBuffer.diffuseColor = color; }
		void SetDiffuseColor(float r, float g, float b, float a) { m_cpuBuffer.diffuseColor = { r, g, b, a }; }

		DirectX::XMFLOAT2 GetTiling() const { return m_cpuBuffer.tiling; }
		void SetTiling(float x, float y) { m_cpuBuffer.tiling = { x, y }; }

		DirectX::XMFLOAT2 GetOffset() const { return m_cpuBuffer.offset; }
		void SetOffset(float x, float y) { m_cpuBuffer.offset = { x, y }; }

		std::shared_ptr<ShaderProgram> GetShaderProgram() const { return m_shaderProgram; }
		void SetShaderProgram(std::shared_ptr<ShaderProgram> program) { m_shaderProgram = program; }
		
		std::string GetShaderProgramName() const;
		void SetShaderProgramByName(const std::string& name);

		eRenderingMode GetRenderingMode() const { return m_renderingMode; }
		void SetRenderingMode(eRenderingMode mode) { m_renderingMode = mode; }

		bool IsTransparent() const;

		static constexpr eResourceType GetType() { return eResourceType::Material; }

	private:
		std::array<std::shared_ptr<Texture2D>, static_cast<size_t>(eMapType::Count)> m_maps;

		MaterialData m_cpuBuffer;
		std::unique_ptr<ConstantBuffer> m_gpuBuffer;

		std::shared_ptr<ShaderProgram> m_shaderProgram;

		eRenderingMode m_renderingMode = eRenderingMode::Opqaue;
	};
}
