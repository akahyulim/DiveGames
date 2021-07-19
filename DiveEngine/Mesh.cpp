#include "Mesh.h"

namespace dive
{
	void Mesh::Clear()
	{
		m_Vertices.clear();
		m_Vertices.shrink_to_fit();

		m_Indices.clear();
		m_Indices.shrink_to_fit();
	}
}