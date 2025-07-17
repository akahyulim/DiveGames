#pragma once

namespace Dive
{
	class StaticMesh;

	class MeshFactory
	{
	public:
		static std::shared_ptr<StaticMesh> CreateTriangle();
		static std::shared_ptr<StaticMesh> CreateQuad();
		static std::shared_ptr<StaticMesh> CreatePlane();
		static std::shared_ptr<StaticMesh> CreateCube();
		static std::shared_ptr<StaticMesh> CreateSphere();
		static std::shared_ptr<StaticMesh> CreateCapsule();

		static std::shared_ptr<StaticMesh> Create(const std::string& meshName);
	};
}
