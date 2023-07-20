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

		DirectX::XMMATRIX skinMatrix[100];
	};

	struct CameraPixelShaderBuffer
	{
		DirectX::XMFLOAT3 cameraPos;
		float poo;
		DirectX::XMFLOAT4 perspectiveValues;
		DirectX::XMMATRIX viewInv;
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

		static void Update();
		static void Render();

		static View* GetView(uint32_t index);
		static void SetView(uint32_t index, View* pView);
		static uint32_t GetViewCount();

		// test
		static ShaderVariation* GetBasicVertexShaderVariation();
		static ShaderVariation* GetBasicPixelShaderVariation();
		static ShaderVariation* GetBasicSkinnedVertexShaderVariation();
		static ShaderVariation* GetBasicSkinnedPixelShaderVariation();
		static ShaderVariation* GetForwardLightVertexShaderVariation();
		static ShaderVariation* GetDirectionalLightPixelShaderVariation();
		static ShaderVariation* GetPointLightPixelShaderVariation();
		static ShaderVariation* GetDeferredShadingVertexShaderVariation();
		static ShaderVariation* GetDeferredShadingPixelShaderVariation();
		static ShaderVariation* GetDeferredDirLightVertexShaderVariation();
		static ShaderVariation* GetDeferredDirLightPixelShaderVariation();

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