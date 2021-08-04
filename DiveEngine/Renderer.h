#pragma once
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include "PipelineState.h"
#include "ConstantBuffer.h"
#include "ConstantBuffers.h"	// �̸� ���� ���
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


	// ���� �� ���� Update�� ����.
	// RenderPath���� ��� �����ϱ� �����̴�.
	// �̴� wicked engine�� ���� �� ���̴�.
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

		// RenderTarget�� ũ���̴�.
		const DirectX::XMINT2& GetResolution() const { return m_RenderTargetSize; }
		void SetResolution(unsigned int width, unsigned int height);

		// update�� draw�� �ִ�.
		// update�� visibility, PerFrameData,  RenderData, CameraCB ���� �ִ�.
		void UpdateCB();
		void DrawScene();

		// �ϴ� ������
		void DrawLegacy();
		void DrawText();

		void ObjectClassify();

		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice.get(); }
		void SetGraphicsDevice(std::shared_ptr<GraphicsDevice> pDevice);
		Texture* GetFrameTexture() { return m_RenderTargets[eRenderTargets::Frame_Ldr]; }

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
		std::shared_ptr<GraphicsDevice> m_pGraphicsDevice;

		// RenderTarget�� ũ���. Ÿ���� �ָ��ϴ�.
		DirectX::XMINT2 m_RenderTargetSize;
		D3D11_VIEWPORT m_ViewPort;

		// GPU Resource ����
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
	};
}