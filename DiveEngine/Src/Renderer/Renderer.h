#pragma once
#include "Graphics/GraphicsDevice.h"
#include "../GraphicsInclude.h"
#include "../GraphicsEnums.h"
#include "../PipelineState.h"
#include "../ConstantBuffer.h"
#include "../ConstantBuffers.h"	// 이름 변경 대상
#include "../Texture.h"
#include "../Font.h"
#include <memory>
#include <string>


namespace dive
{
	class GameObject;
	class TextMesh;
	class dvRenderTexture;

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

		// update와 draw가 있다.
		// update는 visibility, PerFrameData,  RenderData, CameraCB 등이 있다.
		void UpdateCB();

		void ObjectClassify();

		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice; }

		// 이건 Editor용 임시다.
		Texture* GetFrameTexture() { return m_RenderTargets[eRenderTargets::Frame_Ldr]; }
		dvRenderTexture* GetRenderTarget() { return m_pRenderTarget; }

		// Render Passes
		void DrawScene();
		void DrawGBuffer();
		void DrawLight();

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

		// 이건 스파르탄의 GBuffer이다. 지워야 한다.
		std::unordered_map<eRenderTargets, Texture*> m_RenderTargets;
		// 이건 RenderTexture로 GBuffer를 테스트 한 것이다.
		// 이렇게 카메라 없이 단독으로 사용해선 안될 것 같다.
		dvRenderTexture* m_pRenderTarget = nullptr;

		std::unordered_map<eObjectType, std::vector<GameObject*>> m_GameObjects;

		// 이건 고쳐야 된다.
		Font* m_pDvFont = nullptr;

		GameObject* m_pCamera = nullptr;
	};
}