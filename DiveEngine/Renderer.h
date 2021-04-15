#pragma once
#include "GraphicsInclude.h"
#include "GraphicsDevice.h"
#include "GraphicsEnums.h"
#include "PipelineState.h"
#include "Texture.h"
#include <memory>
#include <string>

namespace Dive
{
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
		// frustum�� �ִ�.
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

		// update�� draw�� �ִ�.
		// update�� visibility, PerFrameData,  RenderData, CameraCB ���� �ִ�.
		void UpdateCB();
		void DrawScene();

		// �ϴ� ������
		void DrawColor();
		void DrawTexturing();


		GraphicsDevice* GetGraphicsDevice() { return m_pGraphicsDevice.get(); }
		void SetGraphicsDevice(std::shared_ptr<GraphicsDevice> device);

	private:
		Renderer() = default;
		~Renderer();

		// ���� GPU Resource ����
		bool createDepthStencilStates();
		bool createRasterizerStates();
		bool createSamplerStates();
		bool createConstantBuffers();
		bool createTextures();
		bool createRenderTargetViews();
		bool createShaders();

		void createPipelineStates();

		bool createVertexShader(const std::wstring& filepath, unsigned int shaderType, unsigned int inputLayoutType = ILTYPE_COUNT,
			D3D11_INPUT_ELEMENT_DESC* pDescs = nullptr, UINT numElements = 0);
		bool createPixelShader(const std::wstring& filepath, unsigned int shaderType);

	private:
		std::shared_ptr<GraphicsDevice> m_pGraphicsDevice;

		// GPU Resource ����
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> m_pShaders[SHADERTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayouts[ILTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStates[DSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerStates[RSSTYPE_COUNT];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;

		// constant buffer ����
		struct MatrixBuffer
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
		};
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCBMatrix;

		// pipeline states
		// ���� ���������� �ʿ䰡 ����. enum array�� ������.
		// �׷��� ��Ű��� pipelinestate�� ������ �迭�� �� ����.
		PipelineState m_pipelineStateColor;
		PipelineState m_pipelineStateTexturing;


		// texturing test
		//Texture* m_pTex = nullptr;
		std::shared_ptr<Texture> m_pTex;
		std::shared_ptr<Texture> m_pRTV;
	};
}