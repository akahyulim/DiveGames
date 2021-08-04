#pragma once
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include "PipelineState.h"
#include "ConstantBuffer.h"
#include "ConstantBuffers.h"	// 이름 변경 대상
#include "GBuffer.h"
#include "Texture.h"
#include "Font.h"
#include <memory>
#include <string>

// Collectio of Graphics Technique implementaions.
// Fucntion to draw a scene, shadows, post processes.
// Manager of GraphicsDevice Instance
// Provides other helper functions
namespace dive
{
	class GameObject;
	class TextMesh;

	enum class eRenderTargets
	{
		Gbuffer_Albedo,
		Gbuffer_Normal,
		GBuffer_Material,
		GBuffer_Depth,
		Frame_Ldr,	// 사실 뭔지 모른다.
	};

	// 그려질 대상들
	enum class eObjectType
	{
		Opaque,
		Transparent,
		Light,
		Camera,
	};

	// PipelineState
	// States, Shaders, InputLayout을 설정한 객체
	// 이를 Graphics에 넘겨 한 번에 bind한다.


	// 현재 이 곳엔 Update가 없다.
	// RenderPath에서 취사 선택하기 때문이다.
	// 이는 wicked engine을 참고 한 것이다.
	class Renderer
	{
	public:
		static Renderer& GetInstance()
		{
			static Renderer instance;
			return instance;
		}

		void Initialize();

		const D3D11_VIEWPORT& GetViewport() const { return m_ViewPort; }
		void SetViewport(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f);

		// RenderTarget용 크기이다.
		const DirectX::XMINT2& GetResolution() const { return m_RenderTargetSize; }
		void SetResolution(unsigned int width, unsigned int height);

		// update와 draw가 있다.
		// update는 visibility, PerFrameData,  RenderData, CameraCB 등이 있다.
		void UpdateCB();
		void DrawScene();

		// 일단 나누자
		void DrawLegacy();
		void DrawText();

		void ObjectClassify();

		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice.get(); }
		void SetGraphicsDevice(std::shared_ptr<GraphicsDevice> pDevice);
		Texture* GetFrameTexture() { return m_RenderTargets[eRenderTargets::Frame_Ldr]; }

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
			D3D11_INPUT_ELEMENT_DESC* descs = nullptr, UINT numElements = 0);
		bool createPixelShader(const std::wstring& filepath, unsigned int shaderType);

	private:
		std::shared_ptr<GraphicsDevice> m_pGraphicsDevice;

		// RenderTarget용 크기다. 타입이 애매하다.
		DirectX::XMINT2 m_RenderTargetSize;
		D3D11_VIEWPORT m_ViewPort;

		// GPU Resource 관리
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> m_pShaders[SHADERTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayouts[ILTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStates[DSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerStates[RSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerStateLinear;

		// Constant Buffer
		BufferFrame m_BufferFrameCPU;
		ConstantBuffer* m_pBufferFrameGPU;

		BufferObject m_BufferObjectCPU;
		ConstantBuffer* m_pBufferObjectGPU;

		// constant buffer 부터
		// => 이건 이제 제거 대상이다.
		// 일단 만들어놓은 ConstantBuffer를 적용해보자.
		struct MatrixBuffer
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
			DirectX::XMMATRIX projOrthographic;
		};
		//Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferMatrix;
		ConstantBuffer* m_pBufferFrame;

		// pipeline states
		// 굳이 동적생성할 필요가 없다. enum array로 만들자.
		// 그런데 위키드는 pipelinestate가 종류별 배열로 꽤 많다.
		PipelineState m_PipelineStateColor;
		PipelineState m_PipelineStateTexturing;
		PipelineState m_PipelineStateFont;	// text라는 이름이 더 낫지 않을까?
		PipelineState m_PipelineStateLegacy;

		// RenderTargets 모음이다. 스파르탄의 영향을 받아 GBuffer용으로 구현했다.
		// 현재 Editor에서 Scene을 보여주는 텍스쳐로 활용 중이다.
		// 구현 과정에 의하면 GBuffer와 RenderTarget은 구분되어야 한다.
		std::unordered_map<eRenderTargets, Texture*> m_RenderTargets;

		std::unordered_map<eObjectType, std::vector<GameObject*>> m_GameObjects;

		// 책에서 동적 생성을 하지 않았다.
		// 하지만 생성자에 Device 전달이 필요하다.
		GBuffer* m_pGBuffer;	

		// 이건 고쳐야 된다.
		Font* m_pDvFont = nullptr;
	};
}