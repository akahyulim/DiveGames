#pragma once
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include "PipelineState.h"
#include "Texture.h"
#include "Font.h"
#include <memory>
#include <string>

// Collectio of Graphics Technique implementaions.
// Fucntion to draw a scene, shadows, post processes.
// Manager of GraphicsDevice Instance
// Provides other helper functions
namespace Dive
{
	class TextMesh;

	enum class eRenderTargets
	{
		Gbuffer_Albedo,
		Gbuffer_Normal,
		GBuffer_Material,
		GBuffer_Depth,
		Frame_Ldr,	// 사실 뭔지 모른다.
	};

	enum class eVisibilityType
	{
		Invalid = 0,
		Objects,
		Lights,
	};

	class Visibility
	{
	public:
	private:
		// frustum도 있다.
	};

	// PipelineState
	// States, Shaders, InputLayout을 설정한 객체
	// 이를 Graphics에 넘겨 한 번에 bind한다.

	class Renderer
	{
	public:
		static Renderer& GetInstance()
		{
			static Renderer instance;
			return instance;
		}

		void Initialize();

		const D3D11_VIEWPORT& GetViewport() const { return m_viewport; }
		void SetViewport(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f);

		// RenderTarget용 크기이다.
		const DirectX::XMINT2& GetResolution() const { return m_resolution; }
		void SetResolution(unsigned int width, unsigned int height);

		// update와 draw가 있다.
		// update는 visibility, PerFrameData,  RenderData, CameraCB 등이 있다.
		void UpdateCB();
		void DrawScene();

		// 일단 나누자
		void DrawColor();
		void DrawTexturing();
		void DrawText();


		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice.get(); }
		void SetGraphicsDevice(std::shared_ptr<GraphicsDevice> device);
		Texture* GetFrameTexture() { return m_renderTargets[eRenderTargets::Frame_Ldr]; }

	private:
		Renderer();
		~Renderer();

		// 각종 GPU Resource 생성
		bool createDepthStencilStates();
		bool createRasterizerStates();
		bool createSamplerStates();
		bool createConstantBuffers();
		bool createTextures();	// 의미가 달라졌다.
		bool createRenderTargets();
		bool createShaders();
		bool createFonts();

		void createPipelineStates();

		bool createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType = ILTYPE_COUNT,
			D3D11_INPUT_ELEMENT_DESC* pDescs = nullptr, UINT numElements = 0);
		bool createPixelShader(const std::wstring& filepath, unsigned int shaderType);

	private:
		std::shared_ptr<GraphicsDevice> m_pGraphicsDevice;

		// RenderTarget용 크기다. 타입이 애매하다.
		DirectX::XMINT2 m_resolution;
		D3D11_VIEWPORT m_viewport;

		// GPU Resource 관리
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> m_pShaders[SHADERTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayouts[ILTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStates[DSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerStates[RSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerStateLinear;

		// constant buffer 부터
		struct MatrixBuffer
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
			DirectX::XMMATRIX projOrthographic;
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCBMatrix;

		// pipeline states
		// 굳이 동적생성할 필요가 없다. enum array로 만들자.
		// 그런데 위키드는 pipelinestate가 종류별 배열로 꽤 많다.
		PipelineState m_pipelineStateColor;
		PipelineState m_pipelineStateTexturing;
		PipelineState m_pipelineStateFont;	// text라는 이름이 더 낫지 않을까?

		std::unordered_map<eRenderTargets, Texture*> m_renderTargets;



		// texturing test
		//Texture* m_pTex = nullptr;
		std::shared_ptr<Texture> m_pTex;
		std::shared_ptr<Texture> m_pRTV;
		std::shared_ptr<Texture> m_pCpuTex;

		Font* m_pDvFont = nullptr;
		//TextMesh* m_pTextMesh = nullptr;
	};
}