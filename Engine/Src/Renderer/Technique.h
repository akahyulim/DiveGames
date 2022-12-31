#pragma once
#include "Core/Object.h"
#include "Resource/Resource.h"

namespace Dive
{
	class Context;
	class ShaderVariation;
	class FileStream;

	class Pass
	{
	public:
		explicit Pass(const std::string& name);
		~Pass();

		const std::string& GetName() const { return m_Name; }
		uint32_t GetIndex() const { return m_Index; }

		void SetVertexShader(const std::string& name);
		const std::string& GetVertexShaderName() const { return m_VertexShaderName; }
		
		void SetVertexShaderDefines(const std::string& defines);
		const std::string& GetVertexShaderDefines() const { return m_VertexShaderDefines; }

		void SetPixelShader(const std::string& name);
		const std::string& GetPixelShaderName() const { return m_PixelShaderName; }

		void SetPixelShaderDefines(const std::string& defines);
		const std::string& GetPixelShaderDefines() const { return m_PixelShaderDefines; }

		std::vector<ShaderVariation*>& GetVertexShaderVariations() { return m_VertexShaderVariations; }
		std::vector<ShaderVariation*>& GetPixelShaderVariations() { return m_PixelShaderVariations; }

	private:
		// 결국 이 함수 하나때문에 클래스로 만들었다.
		void releaseShaders();

	private:
		std::string m_Name;
		uint32_t m_Index;

		// 각종 Mode
		// blendMode -> fog
		// cullMode -> Rasterizer State
		// compareMode -> DepthStencil State
		// lightingMode


		std::string m_VertexShaderName;
		std::string m_VertexShaderDefines;
		std::vector<ShaderVariation*> m_VertexShaderVariations;

		std::string m_PixelShaderName;
		std::string m_PixelShaderDefines;
		std::vector<ShaderVariation*> m_PixelShaderVariations;
	};

	class Technique : public Resource
	{
		DIVE_OBJECT(Technique, Resource)

	public:
		explicit Technique(Context* pContext);
		~Technique() override;

		Pass* CreatePass(const std::string& name);
		void RemovePass(const std::string& name);

		Pass* GetPass(uint32_t index) { return index < static_cast<uint32_t>(m_Passes.size()) ? m_Passes[index] : nullptr; }
		Pass* GetPass(const std::string& name);
		
		bool HasPass(uint32_t index) { return index < static_cast<uint32_t>(m_Passes.size()) && m_Passes[index] != nullptr; }
		bool HasPass(const std::string& name);

		// static index들이 있다.
		// base, alpha, material, deferred, light, litBase, litAlpha, shadow 총 8개이다.

		// override
		bool Load(FileStream* pDeserializer) override;

		// static
		static uint32_t GetPassIndex(const std::string& name);
		static void RegisterObject(Context* pContext);

	private:

	private:
		std::vector<Pass*> m_Passes;

		static std::unordered_map<std::string, uint32_t> m_PassIndices;
	};
}