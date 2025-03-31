#pragma once

namespace Dive
{
	class Model;

	class ModelFactory
	{
	public:
		static Model* CreateTriangle(float size);
		static Model* CreateCube(float size);
		static Model* CreatePlane(float length, float depth);
	};
}