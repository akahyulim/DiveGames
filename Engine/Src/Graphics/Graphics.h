#pragma once
#include "Core/Object.h"


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
	class Texture2D;

	// �ʹ� ����. �ٸ� ������ �ű��...?
	// => ���ĸ�ź�� RenderEnums��� ����� �Ű��.
	// ��ü â ȭ�� ������ �ػ󵵿� �� ����� �������� �����.
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

	// ���� �������� �ʴ� ���� �߰��ؾ� �Ѵ�.
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

	// Window, D3D11 RHI �׸��� GPU Resource�� �����մϴ�.
	class Graphics : public Object
	{
		DIVE_OBJECT(Graphics, Object);

	public:
		explicit Graphics(Context* context);
		~Graphics();

		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		bool BeginFrame();
		void EndFrame();

		bool IsInitialized() const;

		// SwapChain
		DirectX::XMUINT2 GetResolution() const;
		void SetResolution(DirectX::XMUINT2 size, bool force);
		void ResizeResolution(DirectX::XMUINT2 size = DirectX::XMUINT2(0, 0));

		// settings�� ���;� �Ѵ�.
		bool SetMode(int width, int height, bool fullScreen, bool borderless, bool vSync);
		bool SetWindowSubclassing(LONG_PTR newProc);

		DirectX::XMUINT2 GetTextureSize() const { return m_textureSize; }
		void ResizeTextures(unsigned int width, unsigned int height);


		std::shared_ptr<Window> GetWindow() const { return m_window; }


		ID3D11Device* GetRHIDevice() const;
		ID3D11DeviceContext* GetRHIContext() const;

		// get
		Shader* GetShader(eRenderShaderType type);
		Shader* GetBaseShader() const { return m_baseShader; }
		ConstantBuffer* GetConstantBuffer() const { return m_constantBuffer; }
		RasterizerState* GetRasterizerState(eRasterizerState state);
		BlendState* GetBlendState(eBlendState state);
		DepthStencilState* GetDepthStencilState(bool enabled);
		Sampler* GetSampler(eSamplerType type);

		// test -> ���߿� map�� key�� ����
		Texture2D* GetEditorTexture() const { return m_editorView; }
		Texture2D* GetDepthStencil() const { return m_depthStencil; }
		
	private:
		Graphics(const Graphics&)				= delete;
		Graphics& operator=(const Graphics&)	= delete;

		bool createDisplay(int width, int height, DisplayMode displayMode);
		void adjustDisplayMode(int& width, int& height, DisplayMode& displayMode);
		
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
		DeviceAndSwapChain* m_deviceAndSwapChain;

		DisplayMode m_displayMode;
		VgaInfo m_vgaInfo;
		std::vector<AdapterInfo> m_adapterInfos;

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

		// textures -> ���߿� enum�� key�� �� unordered_map���� ����
		Texture2D* m_editorView;
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