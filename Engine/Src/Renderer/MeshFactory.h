#pragma once
#include "Mesh.h"
#include "Material.h"

namespace Dive
{
	StaticMesh* CreateQuad(float width, float height);
	StaticMesh* CreateCube(float size);
	StaticMesh* CreatePlane(float width, float height);
	StaticMesh* CreateSphere(float radius = 1.0f, int slices = 20, int stacks = 20);
}