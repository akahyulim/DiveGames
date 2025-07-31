#pragma once

namespace Dive
{
	class StaticMesh;

	class MeshFactory
	{
	public:
		static std::shared_ptr<StaticMesh> CreateTriangle(ID3D11Device* device);
		static std::shared_ptr<StaticMesh> CreateQuad(ID3D11Device* device);
		static std::shared_ptr<StaticMesh> CreatePlane(ID3D11Device* device);
		static std::shared_ptr<StaticMesh> CreateCube(ID3D11Device* device);
		static std::shared_ptr<StaticMesh> CreateSphere(ID3D11Device* device);
		static std::shared_ptr<StaticMesh> CreateCapsule(ID3D11Device* device);

		static std::shared_ptr<StaticMesh> Create(ID3D11Device* device, const std::string& meshName);
	};
}
