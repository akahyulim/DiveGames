#pragma once
#include "RendererDefs.h"

namespace Dive
{
	class RenderTexture;

	class Layer;
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
		DirectX::XMFLOAT4 diffuseColor;

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;
	};

	class Renderer
	{
	public:
		static bool Initialize();
		static void Shutdown();

		static void Update();

		static DirectX::XMFLOAT2 GetResolutionRender();
		static void SetResolutionRender(uint32_t width, uint32_t height);
		static void SetResolutionRender(DirectX::XMINT2 size);

		static RenderTexture* GetRenderTarget(const eRenderTarget renderTarget);

		static ID3D11RasterizerState* GetRasterizerState(const eRasterizerState state);
		static ID3D11DepthStencilState* GetDepthStencilState(const eDepthStencilState state);
		static ID3D11BlendState* GetBlendState(const eBlendState state);

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
		static ShaderVariation* GetDeferredSkinnedShadingVertexShaderVariation();
		static ShaderVariation* GetDeferredSkinnedShadingPixelShaderVariation();
		static ShaderVariation* GetDeferredDirLightVertexShaderVariation();
		static ShaderVariation* GetDeferredDirLightPixelShaderVariation();

		static ConstantBuffer* GetCameraVertexShaderBuffer();
		static ConstantBuffer* GetModelVertexShaderBuffer();
		static ConstantBuffer* GetCameraPixelShaderBuffer();
		static ConstantBuffer* GetLightPixelShaderBuffer();
		static ConstantBuffer* GetMaterialPixelShaderBuffer();

	private:
		static void createRasterizerStates();
		static void createDepthStencilStates();
		static void createBlendStates();
		static bool createShaders();
		static bool createConstantBuffers();

		static void createRenderTargets();
	};
}