#pragma once
#include "Resource/Resource.h"

// ������ ��� �غôµ�...
// ����Ƽ�� �ᱹ ��� ��ҿ� ����Ʈ ���� �ְų� ���� ������� �� ����.
// ���� mesh renderer�� skinned mesh renderer�� ���� �س��� ���̴�.
namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	// Window Styleó�� ����� �ʹ�.
	enum class eVertexFormat
	{
		POSITION,
		COLOR,
		TEX_COORD,
		NORMAL,
	};

	class Dive_Mesh
	{
	public:

		// �� ��� ������ ������ Ÿ���� Ȯ���� �� vertices�� ����
		void GenerateVertices();

	public:
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT4> colors;
		std::vector<DirectX::XMFLOAT2> texCoords;
		std::vector<DirectX::XMFLOAT3> normals;

	private:
		// Ÿ�Ժ� vertices?
		std::vector<unsigned int> indices;
	};

	// mesh data�� ���� ������ ���� �ʿ䰡 ���⶧���� �и��ϴ� �� �´�.
	class Dive_MeshFilter : public Resource
	{
		DIVE_OBJECT(Dive_MeshFilter, Resource);

	public:
		Dive_MeshFilter(Context* context);
		~Dive_MeshFilter();

		// save to file�� vertices, indices�� �ʿ��ϴ�.
		// �׷��� app �󿡼��� ȣ���ϴ� ��찡 ����???
		// ���� �����ڸ� code �󿡼� ���� mesh�� ����ȭ�ϴ� ����ε�...
		// �̰� �ٸ� ���ҽ��鵵 �����غ� ������.
		// import�� ���ҽ��� ���� �������� ����� �� �����غ��� �ѵ�...

		// load from file�� ���� ������ �о ���۸� ����

		void SetMesh(const Dive_Mesh& mesh);

		bool CreateBuffers();

	private:
		void indentifyMeshType();

	private:
		// mesh�� �ƴ϶� vertices, indices�� ������ �Ѵ�.
		// �̰͵��� dive_mesh���� ������� �ϸ� ��������...
		Dive_Mesh m_mesh;

		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
	};
}