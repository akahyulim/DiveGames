#pragma once

namespace Dive
{
	class Mesh;

	class MeshFactory
	{
	public:
		static std::shared_ptr<Mesh> CreateTriangle();
		static std::shared_ptr<Mesh> CreateQuad();
		static std::shared_ptr<Mesh> CreatePlane();
		static std::shared_ptr<Mesh> CreateCube();
		static std::shared_ptr<Mesh> CreateSphere();
		static std::shared_ptr<Mesh> CreateCapsule();
	};
}
