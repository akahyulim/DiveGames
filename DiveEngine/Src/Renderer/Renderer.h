#pragma once

namespace Dive
{
	class View;
	class ShaderVariation;
	class ConstantBuffer;

	struct MatrixBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projMatrix;
	};

	class Renderer
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Update(float delta);
		static void Render();

		static View* GetView(uint32_t index);
		static void SetView(uint32_t index, View* pView);
		static uint32_t GetViewCount();

		// test
		static ShaderVariation* GetVertexShaderVariation();
		static ShaderVariation* GetPixelShaderVariation();
		static ConstantBuffer* GetMatrixBuffer();
		static ID3D11DepthStencilState* GetDepthStencilState();
		static ID3D11RasterizerState* GetRasterizerState();

	private:
		static bool createShaders();
		static bool createConstantBuffers();
		static bool createDepthStencilStates();
		static bool createRasterizerStates();
	};
}