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

		void Initialize();

		const D3D11_VIEWPORT& GetViewport() const { return mViewPort; }
		void SetViewport(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f);

		// RenderTarget용 크기이다.
		const DirectX::XMINT2& GetResolution() const { return mRenderTargetSize; }
		void SetResolution(unsigned int width, unsigned int height);

		// update와 draw가 있다.
		// update는 visibility, PerFrameData,  RenderData, CameraCB 등이 있다.
		void UpdateCB();
		void DrawScene();

		// 일단 나누자
		void DrawColor();
		void DrawTexturing();
		void DrawLegacy();
		void DrawText();

		void ObjectClassify();

		GraphicsDevice* GetGraphicsDevice() { return mGraphicsDevice.get(); }
		void SetGraphicsDevice(std::shared_ptr<GraphicsDevice> device);
		Texture* GetFrameTexture() { return mRenderTargets[eRenderTargets::Frame_Ldr]; }

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
		std::shared_ptr<GraphicsDevice> mGraphicsDevice;

		// RenderTarget용 크기다. 타입이 애매하다.
		DirectX::XMINT2 mRenderTargetSize;
		D3D11_VIEWPORT mViewPort;

		// GPU Resource 관리
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> mShaders[SHADERTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayouts[ILTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilStates[DSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerStates[RSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerStateLinear;

		// constant buffer 부터
		struct MatrixBuffer
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
			DirectX::XMMATRIX projOrthographic;
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferMatrix;

		// pipeline states
		// 굳이 동적생성할 필요가 없다. enum array로 만들자.
		// 그런데 위키드는 pipelinestate가 종류별 배열로 꽤 많다.
		PipelineState mPipelineStateColor;
		PipelineState mPipelineStateTexturing;
		PipelineState mPipelineStateFont;	// text라는 이름이 더 낫지 않을까?
		PipelineState mPipelineStateLegacy;

		std::unordered_map<eRenderTargets, Texture*> mRenderTargets;

		std::unordered_map<eObjectType, std::vector<GameObject*>> mGameObjects;

		// texturing test
		//Texture* mTexture = nullptr;
		std::shared_ptr<Texture> mTexture;
		std::shared_ptr<Texture> mRenderTargetView;
		std::shared_ptr<Texture> mCpuTexture;

		Font* mDvFont = nullptr;
		//TextMesh* m_pTextMesh = nullptr;
	};
}