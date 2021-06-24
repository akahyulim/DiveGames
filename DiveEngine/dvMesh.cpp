#include "dvMesh.h"

namespace dive
{
	void dvMesh::Clear()
	{
		mVertices.clear();
		mVertices.shrink_to_fit();

		mIndices.clear();
		mIndices.shrink_to_fit();
	}
}