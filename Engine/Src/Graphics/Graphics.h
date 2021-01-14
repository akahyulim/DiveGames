#pragma once
#include "Core/Object.h"


namespace Dive
{
	class Context;
	class Window;
	class DepthStencilState;
	class BlendState;
	class RasterizerState;
	class Sampler;
	class Shader;
	class ConstantBuffer;
	class Texture2D;

	// 전체 창 화면 같은건 해상도와 이 모드의 조합으로 만든다.
	enum class eScreenMode
	{
		FullScreen,
		Borderless,
		Windowed,
	};

	struct DisplayMode
	{
		eScreenMode screenMode;
		bool fullScreen;
		bool borderless;
		bool vSync;
		// refresh rate
	};

	struct VgaInfo
	{
		std::wstring name;
		int videoMemory;
	};

	struct AdapterInfo
	{
		AdapterInfo(unsigned int width, unsigned int height, unsigned int numerator, unsigned int denominator)
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

	enum class eRenderShaderType
	{

	};

	// 전부 지원하지 않는 값을 추가해야 한다.
	enum class eRasterizerState
	{
		CullBackSolid,
		CullFrontSolid,
		CullNoneSolid,
		CullBackWireFrame,
		CullFrontWireFrame,
		CullNoneWireFrame,
	};

	enum class eBlendState
	{
		Enabled,
		Disabled,
		ColorAdded,
	};

	enum class eSamplerType
	{
		Linear,
		CompareDepth,
		PointClamp,
		BilinearClamp,
		BilinearWrap,
		TrilinearClamp,
		AnisotropicWrap,
	};

	// Window, D3D11 RHI 그리고 GPU Resource를 관리합니다.
	class Graphics : public Object
	{
		DIVE_OBJECT(Graphics, Object);

	public:
		explicit Graphics(Context* context);
		~Graphics();

		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool BeginFrame();
		void EndFrame();

		// Draw???

		bool IsInitialized();

		// settings를 얻어와야 한다.
		bool SetMode(int width, int height, bool fullScreen, bool borderless, bool vSync);
		bool SetWindowSubclassing(LONG_PTR newProc);

		std::shared_ptr<Window> GetWindow() const { return m_window; }

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		ID3D11Device* GetRHIDevice() const { return m_device; }
		ID3D11DeviceContext* GetRHIContext() const { return m_immediateContext; }

		// get
		Shader* GetShader(eRenderShaderType type);
		Shader* GetBaseShader() const { return m_baseShader; }
		ConstantBuffer* GetConstantBuffer() const { return m_constantBuffer; }
		RasterizerState* GetRasterizerState(eRasterizerState state);
		BlendState* GetBlendState(eBlendState state);
		DepthStencilState* GetDepthStencilState(bool enabled);
		Sampler* GetSampler(eSamplerType type);

		ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView; }

		// test -> 나중엔 map의 key로 접근
		Texture2D* GetRenderTarget() const { return m_renderTarget; }
		Texture2D* GetDepthStencil() const { return m_depthStencil; }
		
	private:
		Graphics(const Graphics&)				= delete;
		Graphics& operator=(const Graphics&)	= delete;

		bool createDisplay(int width, int height, DisplayMode displayMode);
		void adjustDisplayMode(int& width, int& height, DisplayMode& displayMode);
		
		bool createRHI();

		bool createDepthStencilStates();
		bool createBlendStates();
		bool createRasterizerStates();
		bool createSampels();
		bool createShaders();
		bool createTextures();
		bool createBaseShader();
		bool createConstantBuffer();

	private:
		std::shared_ptr<Window> m_window;

		int m_width;
		int m_height;
		DisplayMode m_displayMode;
		VgaInfo m_vgaInfo;
		std::vector<AdapterInfo> m_adapterInfos;

		ID3D11Device* m_device = nullptr;
		ID3D11DeviceContext* m_immediateContext = nullptr;
		IDXGISwapChain* m_swapChain = nullptr;
		ID3D11RenderTargetView* m_renderTargetView = nullptr;

		//= GPU Resources
		// 스파르탄에서는 5개까지 구분해놓았다. => 좀 더 자세히 구분했다.
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

		std::shared_ptr<Sampler> m_samplerLinear;			// test용
		std::shared_ptr<Sampler> m_samplerCompareDepth;
		std::shared_ptr<Sampler> m_samplerPointClamp;
		std::shared_ptr<Sampler> m_samplerBilinearClamp;
		std::shared_ptr<Sampler> m_samplerBilinearWrap;
		std::shared_ptr<Sampler> m_samplerTrilinearClamp;
		std::shared_ptr<Sampler> m_samplerAnisotropicWrap;

		// textures -> 나중엔 enum을 key로 한 unordered_map으로 관리
		Texture2D* m_renderTarget;
		Texture2D* m_depthStencil;

		std::unordered_map<eRenderShaderType, std::shared_ptr<Shader>> m_shaders;
		Shader* m_baseShader;

		struct MatrixBuffer
		{
			DirectX::XMFLOAT4X4 world;
			DirectX::XMFLOAT4X4 view;
			DirectX::XMFLOAT4X4 proj;
		};
		ConstantBuffer* m_constantBuffer;
	};
}