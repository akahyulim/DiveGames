#pragma once

namespace Dive
{
	class DvMesh;

	namespace MeshFactory
	{
		DvMesh* CreateTriangle(float size);
		DvMesh* CreateQuad();
		DvMesh* CreateCube(float size);
		DvMesh* CreatePlane(float length, float depth);
		DvMesh* CreateSphere(float radius = 1.0f, int slices = 20, int stacks = 20);
	}
}