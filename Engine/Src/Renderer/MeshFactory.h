#pragma once

namespace Dive
{
	class Mesh;

	namespace MeshFactory
	{
		Mesh* CreateTriangle(float size);
		Mesh* CreateQuad();
		Mesh* CreateCube(float size);
		Mesh* CreatePlane(float length, float depth);
		Mesh* CreateSphere(float radius = 1.0f, int slices = 20, int stacks = 20);
	}
}