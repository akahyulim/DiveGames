#pragma once

namespace Dive
{
	class ShaderVariation;
	class StaticMesh;
	class Texture2D;
	class ConstantBuffer;

	struct SkydomeConstantBuffer
	{
		DirectX::XMFLOAT4 apexColor;
		DirectX::XMFLOAT4 centerColor;
	};

	// ����Ƽ�� ��� ��Ƽ�����̴�.
	// skybox��� ��Ƽ������ �����ϰ�					=> ��Ƽ������ ������ �� ��ī�̹ڽ��� �����ϰ� ���� �ٽ� ��ü ���̴��� �����Ѵ�.
	// �ٽ� 6sided, cubemap, procedual�� ������.		=> � �ۿ����� ���� ���̴��� Ī�Ѵ�.
	// 6sided�� �ؽ��� 6���� ��ī�̹ڽ��� �����ϰ�
	// ť����� ť��� �ؽ��ķ� ��ī�̹ڽ��� �����Ѵ�.
	// ���������� ���ν����� �¾��� ũ��, ��� ��, �ٴ� �� ���� �����ϴµ� ���� ��ī�̵��� Ȯ���̶� �� �� �ִ�.
	// ���� ���󿡼��� �� ��Ƽ������ ����Ʈ�� �ű�� �ٷ� ����Ǵµ�
	// �Ƹ��� RenderSettings�� ��ϵǴ� �� �ϴ�.		=> RenderSettings Ŭ������ �����ϴµ� �޴��� LightingSettings�� Scene�� ����Ѵ�.
	// ī�޶�� ����� ��ī�̹ڽ��� �� ������ ���ϻ�(��׶���)���� �� ������ ���� ���̴�.
	class Skydome
	{
	public:
		Skydome();
		~Skydome();

		void Update();
		void Render();

		DirectX::XMFLOAT4 GetApexColor() const { return m_ApexColor; }
		void SetApexColor(DirectX::XMFLOAT4 color) { m_ApexColor = color; }

		DirectX::XMFLOAT4 GetCenterColor() const { return m_CenterColor; }
		void SetCenterColor(DirectX::XMFLOAT4 color) { m_CenterColor = color; }

		Texture2D* GetTexture() const { return m_pTexture; }
		void SetTexture(Texture2D* pTex) { m_pTexture = pTex; }


	private:
	private:
		StaticMesh* m_pMesh;
		DirectX::XMFLOAT4 m_ApexColor;
		DirectX::XMFLOAT4 m_CenterColor;
		Texture2D* m_pTexture;
		ConstantBuffer* m_pConstantBuffer;
		ShaderVariation* m_pVertexShader;
		ShaderVariation* m_pPixelShader;
	};
}