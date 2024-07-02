#pragma once
#include "Shader.h"

namespace Dive
{
	//-------------------------------------------------------------------------------------
	// enum class
	//-------------------------------------------------------------------------------------
	enum class eDepthStencilStateType : uint8_t
	{
		Null = 0,
		DepthLess,
		DepthEqual,
		Count
	}; 
	
	enum class eRasterizerStateType : uint8_t
	{
		FillSolid_CullBack = 0,
		FillSolid_CullNone,
		ShadowGen,
		NoDepthClipFront,
		Count
	};

	enum class eBlendStateType : uint8_t
	{
		Null = 0,
		Additive,
		Transparent,
		Count
	}; 
	
	enum class eVSConstBufType : uint8_t
	{
		Model = 0,
		Camera,
		Light,
		Count
	};

	enum class ePSConstBufType : uint8_t
	{
		Model = 0,
		Camera,
		Light,
		Count
	};

	enum class eSamplerType : uint8_t
	{
		Base = 0,
		Pcf,
		Count
	};

	// 이건 사실상 srv type이다.
	enum class eTextureUnitType : uint8_t
	{
		Diffuse = 0,
		Normal,
		Specular,
		GBuffer_DepthStencil,
		GBuffer_Color_SpecIntensity,
		GBuffer_Normal,
		GBuffer_SpecPower,
		SpotShadowMap,
		Count
	};

	enum eClearFlags : uint8_t
	{
		Color = 0x1,
		Depth = 0x2,
		Stencil = 0x4
	};

	enum class eVertexShaderType : uint8_t
	{
		Null = 0,
		LightDepth,
		ForwardLight,
		GBuffer,
		DeferredLight,
		Count
	};

	enum class ePixelShaderType : uint8_t
	{
		Null = 0,
		ForwardLight,
		GBuffer,
		DeferredLight,
		Count
	};

	//-------------------------------------------------------------------------------------
	// constexprs
	//-------------------------------------------------------------------------------------
	inline constexpr uint8_t MAX_NUM_RENDER_VIEWS = 4;
	inline constexpr uint8_t MAX_NUM_VS_CONSTANT_BUFFERS = 6;
	inline constexpr uint8_t MAX_NUM_PS_CONSTANT_BUFFERS = 6;
	inline constexpr uint8_t MAX_NUM_SHADER_RESOURCES = 128;
	inline constexpr uint8_t MAX_NUM_SAMPLERS = 16;

	//-------------------------------------------------------------------------------------
	// structures: 위치가 애매하다. 이 곳에선 사용되지 않지만 다른 곳에선 사용된다.
	//-------------------------------------------------------------------------------------
	struct VertexStatic
	{
		VertexStatic() = default;
		VertexStatic(
			const DirectX::XMFLOAT3& position,
			const DirectX::XMFLOAT2& texCoords,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent,
			const DirectX::XMFLOAT3& bitangent)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->texCoords[0] = texCoords.x;
			this->texCoords[1] = texCoords.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;

			this->bitangent[0] = bitangent.x;
			this->bitangent[1] = bitangent.y;
			this->bitangent[2] = bitangent.z;
		}

		float position[3]{};
		float texCoords[2]{};
		float normal[3]{};
		float tangent[3]{};
		float bitangent[3]{};
	};

	struct VertexSkinned
	{
		VertexSkinned() = default;
		VertexSkinned(
			const DirectX::XMFLOAT3& position,
			const DirectX::XMFLOAT2& texCoords,
			const DirectX::XMFLOAT3& normal,
			const DirectX::XMFLOAT3& tangent,
			const DirectX::XMUINT4& boneIDs,
			const DirectX::XMFLOAT4& weights)
		{
			this->position[0] = position.x;
			this->position[1] = position.y;
			this->position[2] = position.z;

			this->texCoords[0] = texCoords.x;
			this->texCoords[1] = texCoords.y;

			this->normal[0] = normal.x;
			this->normal[1] = normal.y;
			this->normal[2] = normal.z;

			this->tangent[0] = tangent.x;
			this->tangent[1] = tangent.y;
			this->tangent[2] = tangent.z;

			this->boneIDs[0] = boneIDs.x;
			this->boneIDs[1] = boneIDs.y;
			this->boneIDs[2] = boneIDs.z;
			this->boneIDs[3] = boneIDs.w;

			this->weights[0] = weights.x;
			this->weights[1] = weights.y;
			this->weights[2] = weights.z;
			this->weights[3] = weights.w;
		}

		float position[3]{};
		float texCoords[2]{};
		float normal[3]{};
		float tangent[3]{};
		uint32_t boneIDs[4]{};
		float weights[4]{};
	};

	//-------------------------------------------------------------------------------------
	// forward declarations
	//-------------------------------------------------------------------------------------
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class Texture2D;
	class Texture2D;
	class RenderTexture;
	class Shader;
	class ConstantBuffer;
	
	// urho3d는 manages the application window, rendering state and gpu resources라고 설명해 놓았다.
	class Graphics
	{
	public:
		Graphics(const Graphics&) = delete;
		void operator=(const Graphics&) = delete;

		static Graphics* GetInstance()
		{
			if (!s_pInstance)
				s_pInstance = new Graphics;

			return s_pInstance;
		}

		bool Initialize(uint32_t width, uint32_t height, bool fullScreen, bool borderless);
		void Shutdown();

		bool RunWindow();

		LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void AdjustWindow(uint32_t width, uint32_t height, bool borderless);

		bool BeginFrame();
		void EndFrame();

		bool BindRenderTargetView(ID3D11RenderTargetView* pRtv, uint8_t index);
		bool BindRenderTargetView(RenderTexture* pTexture, uint8_t index);
		void BindDepthStencilView(ID3D11DepthStencilView* pDsv);
		void BindDepthStencilView(RenderTexture* pTexture);

		void ClearViews(uint8_t flags, const DirectX::XMFLOAT4& color, float depth = 1.0f, uint8_t stencil = 0);

		void SetDepthStencilState(eDepthStencilStateType type, uint32_t stencilRef = 1);
		void SetRasterizerState(eRasterizerStateType type);
		void SetBlendState(eBlendStateType type);

		void SetViewport(D3D11_VIEWPORT viewport);
		void SetViewport(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

		void SetVertexBuffer(VertexBuffer* pBuffer);
		void SetIndexBuffer(IndexBuffer* pBuffer);

		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

		void SetVertexShader(eVertexShaderType type);
		void SetPixelShader(ePixelShaderType type);
		void BindVSCBuffer(const ConstantBuffer* pBuffer);
		void BindVSConstBuf(eVSConstBufType type);
		void BindPSCBuffer(const ConstantBuffer* pBuffer);
		void BindPSResource(ID3D11ShaderResourceView* pResourceView, eTextureUnitType unit);
		void BindPSResource(Texture* pTexture, eTextureUnitType unit);	// enum class보단 uint8_t를 받는 게 나아보인다.
		void BindPSResource(Texture2D* pTexture, eTextureUnitType unit);
		void BindPSResource(RenderTexture* pRenderTexture, eTextureUnitType unit);
		void BindPSSampler(ID3D11SamplerState* pState, uint8_t index);
		void BindPSSampler(uint8_t index, ID3D11SamplerState* pSampler);

		void SetBoundResources();

		void Draw(uint32_t vertexCount, uint32_t vertexStart = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t indexStart = 0, uint32_t vertexStart = 0);

		bool IsInitialized();

		HINSTANCE GetWindowInstance() { return m_hInstance; }
		HWND GetWindowHandle() { return m_hWnd; }

		std::wstring GetWindowTitle() { return m_WindowTitle; }
		void SetWindowTitle(const std::wstring& title);

		bool IsFullScreen() { return m_bFullScreen; }
		void SetFullScreen(bool enabled);

		uint32_t GetResolutionWidth() { return m_ResolutionWidth; };
		uint32_t GetResolutionHeight() { return m_ResolutionHeight; }
		DirectX::XMFLOAT2 GetResolution() { return { (float)m_ResolutionWidth, (float)m_ResolutionHeight }; }
		void ResizeResolution(uint32_t width, uint32_t height);

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }

		ID3D11RenderTargetView* GetDefaultRenderTargetView() { return m_pDefaultRenderTargetView; }
		ID3D11DepthStencilView* GetDefaultDepthStencilView() { return m_pDefaultDepthStencilView; }
		ID3D11Texture2D* GetDefaultDepthTexture() { return m_pDefaultDepthTexture; }

		bool IsVSync() { return m_bVSync; }
		void SetVSync(bool enabled) { m_bVSync = enabled; }

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() { return m_PrimitiveTopology; }

	private:
		Graphics();
		~Graphics();

		bool createWindow(uint32_t width, uint32_t height, bool borderless);
		bool createDevice(uint32_t width, uint32_t height);
		bool updateSwapChain(uint32_t width, uint32_t height);

		void resetViews();

		bool createDepthStencilStates();
		bool createRasterizerStates();
		bool createBlendStates();

		bool createShaders();

	private:
		static Graphics* s_pInstance;

		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_WindowTitle;
		bool m_bFullScreen;
		uint32_t m_ResolutionWidth;
		uint32_t m_ResolutionHeight;

		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		// 이제 이걸 RenderTexture로 대신할 수 있다.
		// -> 일단 보류. DefaultRenderTarget은 Backbuffer로부터 생성된다.
		ID3D11RenderTargetView* m_pDefaultRenderTargetView;
		ID3D11DepthStencilView* m_pDefaultDepthStencilView;
		ID3D11Texture2D* m_pDefaultDepthTexture;

		bool m_bVSync;	// renderer가 어울릴까?

		//render targets
		ID3D11RenderTargetView* m_RenderTargetViews[MAX_NUM_RENDER_VIEWS];
		ID3D11DepthStencilView* m_pDepthStencilView;
		bool m_bRenderTargetDirty;

		// states
		ID3D11DepthStencilState* m_DepthStencilStates[static_cast<uint8_t>(eDepthStencilStateType::Count)];
		eDepthStencilStateType m_DepthStencilStateType;

		ID3D11RasterizerState* m_RasterizerStates[static_cast<uint8_t>(eRasterizerStateType::Count)];
		eRasterizerStateType m_RasterizerStateType;
		
		ID3D11BlendState* m_BlendStates[static_cast<uint8_t>(eBlendStateType::Count)];
		eBlendStateType m_BlendStateType;

		D3D11_VIEWPORT m_Viewport;

		// vertex shader
		Shader* m_VertexShaders[static_cast<uint8_t>(eVertexShaderType::Count)];
		eVertexShaderType m_VertexShaderType;

		// vertex shader constant buffers
		// 생성된 버퍼와 바인드될 버퍼 공간 둘로 구분된다.
		ConstantBuffer* m_VSConstBufs[static_cast<uint8_t>(eVSConstBufType::Count)];
		ID3D11Buffer* m_VSConstBufSlots[MAX_NUM_VS_CONSTANT_BUFFERS];
		bool m_bVSConstBufDirty;

		// pixel shader
		Shader* m_PixelShaders[static_cast<uint8_t>(ePixelShaderType::Count)];
		ePixelShaderType m_PixelShaderType;

		// pixel shader constant buffers
		ConstantBuffer* m_PSConstBufs[static_cast<uint8_t>(ePSConstBufType::Count)];
		ID3D11Buffer* m_PSConstBufSlots[MAX_NUM_PS_CONSTANT_BUFFERS];
		bool m_bPSConstBufDirty;

		// pixel shader resources
		ID3D11ShaderResourceView* m_PSResourceViews[MAX_NUM_SHADER_RESOURCES];
		bool m_bPSResourceViewDirty;
		uint8_t m_PSResourceViewDirtyFirst;
		uint8_t m_PSResourceViewDirtyLast;

		// sampler
		std::array<ID3D11SamplerState*, static_cast<uint8_t>(eSamplerType::Count)> m_PSSamplers;
		bool m_bPSSamplerDirty;
		uint8_t m_PSSamplerDirtyFirst;
		uint8_t m_PSSamplerDirtyLast;

		// vertex buffers
		VertexBuffer* m_pVertexBuffer;

		// index buffer
		IndexBuffer* m_pIndexBuffer;

		// primirive topology
		D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;
	};
}