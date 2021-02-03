#pragma once
#include "Component.h"
#include "Graphics/MeshFilter.h"


namespace Dive
{
	class Context;
	class GameObject;
	class Material;

	// Rigid, Skinned, etc�� ������ �� �ִ�.
	// �̰� ����Ƽ�� ���� ���� �� �� ����.
	class Renderable : public Component
	{
		DIVE_OBJECT(Renderable, Component);

	public:
		explicit Renderable(Context* context, GameObject* owner);
		~Renderable();

		MeshFilter* GetMeshFilter() const			{ return m_meshFilter; }
		void SetMeshFilter(MeshFilter* meshFilter)	{ m_meshFilter = meshFilter; }

		Material* GetMaterial() const				{ return m_material; }
		void SetMaterial(Material* material);

		bool IsEnabled() const						{ return m_bEnabled; }
		void SetEnable(bool enable)					{ m_bEnabled = enable; }

		unsigned int GetVertexBufferOffset() const	{ return m_vertexBufferOffset; }
		unsigned int GetVertexCount() const			{ return m_meshFilter ? m_meshFilter->GetVertexCount() : 0; }
		unsigned int GetIndexBufferOffset() const	{ return m_indexBufferOffset; }
		unsigned int GetIndexCount() const			{ return m_meshFilter ? m_meshFilter->GetIndexCount() : 0; }

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const					{ return m_primitiveTopology; }
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology)	{ m_primitiveTopology = primitiveTopology; }


	private:
		Renderable(const Renderable&)				= delete;
		Renderable& operator=(const Renderable&)	= delete;

	private:
		bool m_bEnabled;

		// �� �� ���ҽ��ε�... enum���� �����ϴ°� ���� ��������?
		// MeshFilter�� enum���� �����ϱ� ��ưڴ�...
		MeshFilter* m_meshFilter;
		Material* m_material;

		// ���������� offset�� ��ü�� �𸣰ڴ�.
		// �ٸ� count�� MeshFilet�� �Ű��.
		unsigned int m_vertexBufferOffset;
		unsigned int m_indexBufferOffset;

		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;
	};
}