#pragma once

namespace Dive
{
	class Model;

	namespace ModelFactory
	{
		Model* CreateTriangle(float size);
		Model* CreateCube(float size);
		Model* CreatePlane(float length, float depth);
	}
}