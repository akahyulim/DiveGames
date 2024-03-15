#pragma once

namespace Dive
{
	class Model;

	namespace ModelFactory
	{
		Model* CreateTriangle(float size);
		//Model* CreateQuad();
		Model* CreateCube(float size);
		Model* CreatePlane(float length, float depth);
		Model* CreateSphere(float radius = 1.0f, int slices = 20, int stacks = 20);
	}
}