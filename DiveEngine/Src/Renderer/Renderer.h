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

	struct CameraVertexShaderBuffer
	{
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projMatrix;
	};

	struct ModelVertexShaderBuffer
	{
		DirectX::XMMATRIX worldMatrix;
	};

	struct CameraPixelShaderBuffer
	{
		DirectX::XMFLOAT3 cameraPos;
		float poo;
	};

	struct LightPixelShaderBuffer
	{
		DirectX::XMFLOAT3 lightPos;
		float lightRange;
		DirectX::XMFLOAT3 lightColor;
		float lightSpotAngle;
		DirectX::XMFLOAT3 lightDir;
		float poo;
	};

	struct MaterialPixelShaderBuffer
	{
		DirectX::XMFLOAT4 diffColor;
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
		static ShaderVariation* GetForwardLightVertexShaderVariation();
		static ShaderVariation* GetDirectionalLightPixelShaderVariation();
		static ShaderVariation* GetPointLightPixelShaderVariation();

		static ConstantBuffer* GetCameraVertexShaderBuffer();
		static ConstantBuffer* GetModelVertexShaderBuffer();
		static ConstantBuffer* GetCameraPixelShaderBuffer();
		static ConstantBuffer* GetLightPixelShaderBuffer();
		static ConstantBuffer* GetMaterialPixelShaderBuffer();
		
		static ID3D11DepthStencilState* GetDepthStencilState();
		static ID3D11DepthStencilState* GetForwardLightDS();

		static ID3D11RasterizerState* GetRasterizerState();

		static ID3D11BlendState* GetBlendState();

	private:
		static bool createShaders();
		static bool createConstantBuffers();
		static bool createDepthStencilStates();
		static bool createRasterizerStates();
		static bool createBlendStates();
	};
}