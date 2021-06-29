#include "Mesh.h"

namespace dive
{
	void Mesh::Clear()
	{
		mVertices.clear();
		mVertices.shrink_to_fit();

		mIndices.clear();
		mIndices.shrink_to_fit();
	}
}