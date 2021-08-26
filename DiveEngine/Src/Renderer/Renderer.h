#pragma once
#include "Graphics/GraphicsDevice.h"
#include "../GraphicsInclude.h"
#include "../GraphicsEnums.h"
#include "../PipelineState.h"
#include "../ConstantBuffer.h"
#include "../ConstantBuffers.h"	// �̸� ���� ���
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

		void Initialize(HWND hWnd, bool fullScreen);

		const D3D11_VIEWPORT& GetViewport() const { return m_Viewport; }
		void SetViewport(float width, float height, float offsetX = 0.0f, float offsetY = 0.0f);

		// ���� Editor�� Scene���� RenderTexture�� ũ�⸦ ������ �� ��� ���̴�.
		const DirectX::XMINT2& GetResolution() const { return m_RenderTargetSize; }
		void SetResolution(unsigned int width, unsigned int height);

		void GetGBufferSize(unsigned int& outWidth, unsigned int& outHeight);

		// update�� draw�� �ִ�.
		// update�� visibility, PerFrameData,  RenderData, CameraCB ���� �ִ�.
		void UpdateCB();

		//= ��׵��� �Ʒ� Pass���� ����ؾ� �Ѵ�. =============
		// �׷��� �����غ��� ���ĸ�ź�� Draw�� Pass�� ���� ������ �� ������...?
		// ã�ƺ��� ���ĸ�ź�� DrawXXX�� ���� Debug�� �׸����.
		void DrawScene();

		// �ϴ� ������
		void DrawLegacy();
		void DrawText();
		//=====================================================

		void ObjectClassify();

		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice; }

		// �̰� Editor�� �ӽô�.
		Texture* GetFrameTexture() { return m_RenderTargets[eRenderTargets::Frame_Ldr]; }

		// Render Passes
		// RenderPath���� ȣ���� �ű� ������ public�̾�� �Ѵ�.
		// Backbuffer, GBuffer�� RenderTargetView ������ �̵� ������ ���ԵǾ�� �Ѵ�.
		// ������ Pass�� Path�� ������ ��Ȯ���� �ʴ�.
		// �� ������ Pass�� ������ �ٽ� Path���� ��缱���ϴ°� �����Ѱ�?
		void BeginScene();
		void PassGBuffer();
		void PassLighting();

		ID3D11DepthStencilState* GetDepthStencilState(eDepthStencilState state) { return m_pDepthStencilStates[static_cast<int>(state)].Get(); }
		ID3D11RasterizerState* GetRasterizerState(eRasterizerState state) { return m_pRasterizerStates[static_cast<int>(state)].Get(); }
		ID3D11SamplerState* GetSamplerState(eSamplerState state) { return m_pSamplerStates[static_cast<int>(state)].Get(); }

		// �ӽ�
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

		// RenderTarget�� ũ���. Ÿ���� �ָ��ϴ�.
		DirectX::XMINT2 m_RenderTargetSize;

		D3D11_VIEWPORT m_Viewport;

		// GPU Resource ����
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

		// constant buffer ����
		// => �̰� ���� ���� ����̴�.
		// �ϴ� �������� ConstantBuffer�� �����غ���.
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
		// ���� ���������� �ʿ䰡 ����. enum array�� ������.
		// �׷��� ��Ű��� pipelinestate�� ������ �迭�� �� ����.
		PipelineState m_PipelineStateColor;
		PipelineState m_PipelineStateTexturing;
		PipelineState m_PipelineStateFont;	// text��� �̸��� �� ���� ������?
		PipelineState m_PipelineStateLegacy;

		// RenderTargets �����̴�. ���ĸ�ź�� ������ �޾� GBuffer������ �����ߴ�.
		// ���� Editor���� Scene�� �����ִ� �ؽ��ķ� Ȱ�� ���̴�.
		// ���� ������ ���ϸ� GBuffer�� RenderTarget�� ���еǾ�� �Ѵ�.
		std::unordered_map<eRenderTargets, Texture*> m_RenderTargets;

		std::unordered_map<eObjectType, std::vector<GameObject*>> m_GameObjects;

		// å���� ���� ������ ���� �ʾҴ�.
		// ������ �����ڿ� Device ������ �ʿ��ϴ�.
		GBuffer* m_pGBuffer;	

		// �̰� ���ľ� �ȴ�.
		Font* m_pDvFont = nullptr;

		GameObject* m_pCamera = nullptr;
	};
}