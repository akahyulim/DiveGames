#pragma once
#include "Graphics/GraphicsDevice.h"
#include "../GraphicsInclude.h"
#include "../GraphicsEnums.h"
#include "../PipelineState.h"
#include "../ConstantBuffer.h"
#include "../ConstantBuffers.h"	// 이름 변경 대상
#include "../GBuffer.h"
#include "../Texture.h"
#include "../Font.h"
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


	class Renderer
	{
	public:
		static Renderer& GetInstance()
		{
			static Renderer instance;
			return instance;
		}

		void Initialize(HWND hWnd, bool fullScreen);

		const D3D11_VIEWPORT& GetViewport() const { return m_Viewport; }
		void SetViewport(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f);

		// 현재 Editor의 Scene에서 RenderTexture의 크기를 변경할 때 사용 중이다.
		const DirectX::XMINT2& GetResolution() const { return m_RenderTargetSize; }
		void SetResolution(unsigned int width, unsigned int height);

		void GetGBufferSize(unsigned int& outWidth, unsigned int& outHeight);

		// update와 draw가 있다.
		// update는 visibility, PerFrameData,  RenderData, CameraCB 등이 있다.
		void UpdateCB();

		//= 얘네들은 아래 Pass들이 흡수해야 한다. =============
		// 그런데 생각해보니 스파르탄도 Draw와 Pass가 따로 구현된 것 같은데...?
		// 찾아보니 스파르탄의 DrawXXX는 전부 Debug용 그리기다.
		void DrawScene();

		// 일단 나누자
		void DrawLegacy();
		void DrawText();
		//=====================================================

		void ObjectClassify();

		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice; }

		// 이건 Editor용 임시다.
		Texture* GetFrameTexture() { return m_RenderTargets[eRenderTargets::Frame_Ldr]; }

		// Render Passes
		// RenderPath에서 호출할 거기 때문에 public이어야 한다.
		// Backbuffer, GBuffer의 RenderTargetView 설정은 이들 구현에 포함되어야 한다.
		// 아직도 Pass와 Path의 구분이 명확하지 않다.
		// 이 곳에서 Pass를 나누고 다시 Path에서 취사선택하는게 가능한가?
		void BeginScene();
		void PassGBuffer();
		void PassLighting();

		ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState state) { return m_pDepthStencilStates[static_cast<int>(state)].Get(); }
		ID3D11RasterizerState* GetRasterizerState(eRasterizerState state) { return m_pRasterizerStates[static_cast<int>(state)].Get(); }
		ID3D11SamplerState* GetSamplerState(eSamplerState state) { return m_pSamplerStates[static_cast<int>(state)].Get(); }

		// 임시
		GameObject* GetCamera() { return m_pCamera; }

	private:
		Renderer();
		~Renderer();

		// Render Resources
		void createDepthStencilStates();
		void createRasterizerStates();
		void createSamplerStates();

		bool createConstantBuffers();
		bool createRenderTargets();
		bool createShaders();
		bool createFonts();

		void createPipelineStates();

		bool createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType = ILTYPE_COUNT,
			D3D11_INPUT_ELEMENT_DESC* descs = nullptr, UINT numElements = 0);
		bool createPixelShader(const std::wstring& filepath, unsigned int shaderType);


	private:
		GraphicsDevice* m_pGraphicsDevice = nullptr;

		// RenderTarget용 크기다. 타입이 애매하다.
		DirectX::XMINT2 m_RenderTargetSize;

		D3D11_VIEWPORT m_Viewport;

		// GPU Resource 관리
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> m_pShaders[SHADERTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayouts[ILTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStates[static_cast<int>(eDepthStencilState::Count)];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerStates[static_cast<int>(eRasterizerState::Count)];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerStates[static_cast<int>(eSamplerState::Count)];
		//Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerStateLinear;

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

		GameObject* m_pCamera = nullptr;
	};
}