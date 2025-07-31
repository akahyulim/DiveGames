#pragma once
#include "Shader.h"
#include "InputLayoutKey.h"
#include "ShaderProgram.h"


namespace Dive
{
	// 유니티의 경우 InputLayout은 Mesh가 관리한다고...
	class ShaderManager
	{
	public:
		static bool Initialize(ID3D11Device* device);
		static void Clear();

		static bool CreateInputLayout(const VertexShader* vs, eInputLayout type, ID3D11Device* device);

		static IShader* GetShader(const std::string& name);
		static ID3D11InputLayout* GetInputLayout(const std::string& shaderName, eInputLayout inputLayoutType);
		static void BindInputLayout(const std::string& shaderName, eInputLayout inputLayoutType, ID3D11DeviceContext* deviceContext);

	private:
		static std::unordered_map<std::string, std::unique_ptr<IShader>> s_shaders;
		static std::unordered_map<InputLayoutKey, Microsoft::WRL::ComPtr<ID3D11InputLayout>> s_inputLayouts;

		// shader programs
		// standard: pbs
		// legacy
		// unlit
		// particle
		// skybox
		// sprite
		// terrain
	};
}
