#pragma once
#include "Graphics/Graphics.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eRenderPathType : uint8_t
	{
		Forward = 0,
		Deferred
	};

	enum class eRenderTargetType : uint8_t
	{
		GBuffer_DepthStencil = 0,
		GBuffer_Color_SpecIntensity,
		GBuffer_Normal,
		GBuffer_SpecPower,
		FrameRender,
		FrameOutput,
		Count
	};

	//-------------------------------------------------------------------------------------
	// structures: VS, PS Constant Buffers
	//-------------------------------------------------------------------------------------
	struct VSConstBuf_Model
	{
		DirectX::XMMATRIX world;
	};

	struct VSConstBuf_Camera
	{
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct VSConstBuf_Light
	{
		DirectX::XMMATRIX shadow;
	};

	struct PSConstBuf_Model
	{
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 normal;	// xmfloat3�̾�� �ϳ�...

		DirectX::XMFLOAT2 tiling;
		DirectX::XMFLOAT2 offset;

		uint32_t properties;
		DirectX::XMFLOAT3 padding;
	};

	struct PSConstBuf_Camera
	{
		DirectX::XMFLOAT3 position;
		float padding;

		DirectX::XMFLOAT4 perspectiveValue;

		DirectX::XMMATRIX viewInverse;
	};

	struct PSConstBuf_Light
	{
		DirectX::XMFLOAT3 color;
		float outerConeAngle;

		DirectX::XMFLOAT3 position;
		float rangeRcp;

		DirectX::XMFLOAT3 direction;
		float innerConeAngle;

		uint32_t options;
		int shadowEnabled;
		DirectX::XMFLOAT2 padding;

		DirectX::XMMATRIX shadow;
	};

	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class ViewScreen;
	class RenderTexture;
	class ConstantBuffer;
	class Shader;

	// urho3d�� ����Ʈ�� ���� ������ ����(�׸���, ��ũ��, ���͸��, �׸��� �� ������ ��)�� �����ϰ�
	// �̵��� �̿��� update�� render�� �����Ѵ�.
	// ��Ȯ�ϰԴ� manages drawing of 3d views��� ������ ���Ҵ�.
	// ������ urho3dó�� ������ © �� ���ҽ����� ������ ���� �ϴ����̴�.
	// => ��� ��Ҵ� Graphics���� �������� �����ϸ� ���� �����ϴ� �� �ϴ�.
	class Renderer
	{
	public:
		Renderer(const Renderer&) = delete;
		void operator=(const Renderer&) = delete;

		static Renderer* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Renderer;

			return s_pInstance;
		}

		bool Initialize();
		void Shutdown();

		void Update();
		void Render();

		uint32_t GetNumViewScreens() { return static_cast<uint32_t>(m_ViewScreens.size()); }
		void SetNumViewScreens(uint32_t count);

		ViewScreen* GetViewScreen(uint32_t index);
		void SetViewScreen(uint32_t index, ViewScreen* pViewScreen);

		ConstantBuffer* GetVSConstantBuffer(eVSConstBufType type) const { return m_VSConstantBuffers[static_cast<uint8_t>(type)]; }
		ConstantBuffer* GetPSConstantBuffer(ePSConstBufType type) const { return m_PSConstantBuffers[static_cast<uint8_t>(type)]; }

		ID3D11SamplerState* GetSampler(eSamplerType type) const { return m_Samplers[static_cast<uint8_t>(type)]; }

		bool IsInitialized() const { return m_bInitialized; }

	private:
		Renderer();
		~Renderer();

		bool createRenderTextures();
		bool createConstantBuffers();
		bool createSamplers();

	private:
		static Renderer* s_pInstance;

		ID3D11Device* m_pDevice;

		std::vector<ViewScreen*> m_ViewScreens;	// ����Ƽ�� ���̾�ʹ� �ٸ���. �������� ����.

		std::array<RenderTexture*, static_cast<uint8_t>(eRenderTargetType::Count)> m_RenderTargets;

		std::array<ConstantBuffer*, static_cast<uint8_t>(eVSConstBufType::Count)> m_VSConstantBuffers;
		std::array<ConstantBuffer*, static_cast<uint8_t>(ePSConstBufType::Count)> m_PSConstantBuffers;
		
		std::array<ID3D11SamplerState*, static_cast<uint8_t>(eSamplerType::Count)> m_Samplers;

		bool m_bInitialized;
	};

	Renderer* GetRenderer();
}