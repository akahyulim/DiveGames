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
		Frame_Ldr,	// ��� ���� �𸥴�.
	};

	// �׷��� ����
	enum class eObjectType
	{
		Opaque,
		Transparent,
		Light,
		Camera,
	};

	// PipelineState
	// States, Shaders, InputLayout�� ������ ��ü
	// �̸� Graphics�� �Ѱ� �� ���� bind�Ѵ�.

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

		// RenderTarget�� ũ���̴�.
		const DirectX::XMINT2& GetResolution() const { return mRenderTargetSize; }
		void SetResolution(unsigned int width, unsigned int height);

		// update�� draw�� �ִ�.
		// update�� visibility, PerFrameData,  RenderData, CameraCB ���� �ִ�.
		void UpdateCB();
		void DrawScene();

		// �ϴ� ������
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

		// ���� GPU Resource ����
		bool createDepthStencilStates();
		bool createRasterizerStates();
		bool createSamplerStates();
		bool createConstantBuffers();
		bool createTextures();	// �ǹ̰� �޶�����.
		bool createRenderTargets();
		bool createShaders();
		bool createFonts();

		void createPipelineStates();

		bool createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType = ILTYPE_COUNT,
			D3D11_INPUT_ELEMENT_DESC* descs = nullptr, UINT numElements = 0);
		bool createPixelShader(const std::wstring& filepath, unsigned int shaderType);

	private:
		std::shared_ptr<GraphicsDevice> mGraphicsDevice;

		// RenderTarget�� ũ���. Ÿ���� �ָ��ϴ�.
		DirectX::XMINT2 mRenderTargetSize;
		D3D11_VIEWPORT mViewPort;

		// GPU Resource ����
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> mShaders[SHADERTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayouts[ILTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilStates[DSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerStates[RSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerStateLinear;

		// constant buffer ����
		struct MatrixBuffer
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
			DirectX::XMMATRIX projOrthographic;
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferMatrix;

		// pipeline states
		// ���� ���������� �ʿ䰡 ����. enum array�� ������.
		// �׷��� ��Ű��� pipelinestate�� ������ �迭�� �� ����.
		PipelineState mPipelineStateColor;
		PipelineState mPipelineStateTexturing;
		PipelineState mPipelineStateFont;	// text��� �̸��� �� ���� ������?
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