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
		DirectX::XMFLOAT4 cameraPos;
	};

	struct LightPixelShaderBuffer
	{
		DirectX::XMFLOAT4 lightColor;
		DirectX::XMFLOAT4 lightDir;
		float lightRange;
		float lightSpotAngle;
		float dump[2] = { 0.0, };	// 4byte로 나누어떨어지지 않으면 버퍼 생성시 오류가 발생한다.
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

		static ConstantBuffer* GetCameraVertexShaderBuffer();
		static ConstantBuffer* GetModelVertexShaderBuffer();
		static ConstantBuffer* GetCameraPixelShaderBuffer();
		static ConstantBuffer* GetLightPixelShaderBuffer();
		
		static ID3D11DepthStencilState* GetDepthStencilState();
		static ID3D11RasterizerState* GetRasterizerState();

	private:
		static bool createShaders();
		static bool createConstantBuffers();
		static bool createDepthStencilStates();
		static bool createRasterizerStates();
	};
}