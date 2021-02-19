#pragma once
#include "Core/Object.h"
#include "GraphicsEnums.h"


namespace Dive
{
	class Context;
	class Window;
	class DeviceAndSwapChain;
	class DepthStencilState;
	class BlendState;
	class RasterizerState;
	class Sampler;
	class Shader;
	class ConstantBuffer;
	class Dive_Texture;
	class GBuffer;

	struct DISPLAY_MODE
	{
		eScreenMode screenMode;
		bool fullScreen;
		bool borderless;
		bool vSync;
		// refresh rate
	};

	struct VGA_INFO
	{
		std::wstring name;
		int videoMemory;
	};

	struct ADAPTER_INFO
	{
		ADAPTER_INFO(unsigned int width, unsigned int height, unsigned int numerator, unsigned int denominator)
		{
			this->width = width;
			this->hegiht = height;
			this->numerator = numerator;
			this->denominator = denominator;
		}

		unsigned int width;
		unsigned int hegiht;
		unsigned int numerator;
		unsigned int denominator;
	};

	// Window, D3D11 RHI �׸��� GPU Resource�� �����մϴ�.
	class Graphics : public Object
	{
		DIVE_OBJECT(Graphics, Object);

	public:
		explicit Graphics(Context* context);
		~Graphics();

		// settings�� ���;� �Ѵ�.
		bool SetMode(int width, int height, bool fullScreen, bool borderless, bool vSync);
		bool SetWindowSubclassing(LONG_PTR newProc);
		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool BeginFrame();
		void EndFrame();

		bool IsInitialized() const;

		// SwapChain
		DirectX::XMUINT2 GetResolution() const;
		void SetResolution(DirectX::XMUINT2 size, bool force);
		void ResizeResolution(DirectX::XMUINT2 size = DirectX::XMUINT2(0, 0));
		ID3D11RenderTargetView* GetRenderTargetView(); 	// �̸� ���� �ʿ�?

		// RenderTargets
		//Dive_Texture* GetRenderTexture(eRenderTextureType type) { return m_renderTextures[type].get(); }
		//DirectX::XMUINT2 GetTextureSize() const { return m_textureSize; }
		//void ResizeTextures(const DirectX::XMUINT2& size);

		std::shared_ptr<Window> GetWindow() const { return m_window; }
		ID3D11Device* GetRHIDevice() const;
		ID3D11DeviceContext* GetRHIContext() const;

		// get
		Shader* GetShader(eRenderShaderType type) { return m_shaders[type].get(); }
		RasterizerState* GetRasterizerState(eRasterizerState state);
		BlendState* GetBlendState(eBlendState state);
		DepthStencilState* GetDepthStencilState(bool enabled);
		Sampler* GetSampler(eSamplerType type);

		GBuffer* GetGBuffer() const { return m_gbuffer.get(); }

		// constant buffers
		ConstantBuffer* GetObjectContantBuffer() { return m_cbObject.get(); }

	private:
		Graphics(const Graphics&)				= delete;
		Graphics& operator=(const Graphics&)	= delete;

		bool createDisplay(int width, int height, DISPLAY_MODE displayMode);
		void adjustDisplayMode(int& width, int& height, DISPLAY_MODE& displayMode);
		
		bool createDepthStencilStates();
		bool createBlendStates();
		bool createRasterizerStates();
		bool createSampels();
		bool createShaders();
		bool createTextures();
		bool createConstantBuffers();

	private:
		std::shared_ptr<Window> m_window;
		std::shared_ptr<DeviceAndSwapChain> m_deviceAndSwapChain;

		DISPLAY_MODE m_displayMode;
		VGA_INFO m_vgaInfo;
		std::vector<ADAPTER_INFO> m_adapterInfos;

		// texture size
		DirectX::XMUINT2 m_textureSize;

		//= GPU Resources
		// ���ĸ�ź������ 5������ �����س��Ҵ�. => �� �� �ڼ��� �����ߴ�.
		std::shared_ptr<DepthStencilState> m_depthStencilStateEnabled;
		std::shared_ptr<DepthStencilState> m_depthStencilStateDisabled;

		std::shared_ptr<BlendState> m_blendStateEnable;
		std::shared_ptr<BlendState> m_blendStateDisable;
		std::shared_ptr<BlendState> m_blendStateColorAdd;

		std::shared_ptr<RasterizerState> m_rasterizeStateCullBackSolid;
		std::shared_ptr<RasterizerState> m_rasterizeStateCullFrontSolid;
		std::shared_ptr<RasterizerState> m_rasterizeStateCullNoneSolid;
		std::shared_ptr<RasterizerState> m_rasterizeStateCullBackWireFrame;
		std::shared_ptr<RasterizerState> m_rasterizeStateCullFrontWireFrame;
		std::shared_ptr<RasterizerState> m_rasterizeStateCullNoneWireFrame;

		std::shared_ptr<Sampler> m_samplerLinear;			// test��
		std::shared_ptr<Sampler> m_samplerCompareDepth;
		std::shared_ptr<Sampler> m_samplerPointClamp;
		std::shared_ptr<Sampler> m_samplerBilinearClamp;
		std::shared_ptr<Sampler> m_samplerBilinearWrap;
		std::shared_ptr<Sampler> m_samplerTrilinearClamp;
		std::shared_ptr<Sampler> m_samplerAnisotropicWrap;

		std::unordered_map<eRenderShaderType, std::shared_ptr<Shader>> m_shaders;

		std::shared_ptr<ConstantBuffer> m_cbObject;

		// �̷��� �����ҰŸ� �ٸ� ���ҽ��鵵 �����°� �� ������� ������?
		std::shared_ptr<GBuffer> m_gbuffer;
	};
}