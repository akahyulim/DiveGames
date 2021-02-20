#pragma once
#include "Resource/Resource.h"

// 생각을 계속 해봤는데...
// 유니티는 결국 모든 요소에 디폴트 값을 넣거나 직접 계산했을 것 같다.
// 물론 mesh renderer와 skinned mesh renderer는 구분 해놓고 말이다.
namespace Dive
{
	class VertexBuffer;
	class IndexBuffer;

	// Window Style처럼 만들고 싶다.
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

		// 각 요소 정보를 취합해 타입을 확정한 후 vertices를 구성
		void GenerateVertices();

	public:
		std::vector<DirectX::XMFLOAT3> positions;
		std::vector<DirectX::XMFLOAT4> colors;
		std::vector<DirectX::XMFLOAT2> texCoords;
		std::vector<DirectX::XMFLOAT3> normals;

	private:
		// 타입별 vertices?
		std::vector<unsigned int> indices;
	};

	// mesh data는 굳이 가지고 있을 필요가 없기때문에 분리하는 게 맞다.
	class Dive_MeshFilter : public Resource
	{
		DIVE_OBJECT(Dive_MeshFilter, Resource);

	public:
		Dive_MeshFilter(Context* context);
		~Dive_MeshFilter();

		// save to file은 vertices, indices가 필요하다.
		// 그런데 app 상에서는 호출하는 경우가 없다???
		// 굳이 따지자면 code 상에서 만든 mesh를 파일화하는 경우인데...
		// 이건 다른 리소스들도 생각해볼 문제다.
		// import한 리소스를 엔진 포멧으로 만드는 건 생각해볼만 한데...

		// load from file은 엔진 포멧을 읽어서 버퍼를 생성

		void SetMesh(const Dive_Mesh& mesh);

		bool CreateBuffers();

	private:
		void indentifyMeshType();

	private:
		// mesh가 아니라 vertices, indices를 가져야 한다.
		// 이것들을 dive_mesh에서 관리토록 하면 좋을지도...
		Dive_Mesh m_mesh;

		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
	};
}