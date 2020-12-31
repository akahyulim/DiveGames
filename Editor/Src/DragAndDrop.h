#pragma once
#include "ImGUI/imgui.h"
#include <variant>

enum class eDragPayloadType
{
	Unknown,
	Actor,
	Texture,
	Model,
	Script,
};

struct DragDropPayload
{
	// variant�� ����ü. ��, DataVariant�� const char* Ȥ�� unsigned int�� ���� �� �ִ�.
	using DataVariant = std::variant<const char*, unsigned int>;

	DragDropPayload(eDragPayloadType type = eDragPayloadType::Unknown, DataVariant data = nullptr)
	{
		this->type = type;
		this->data = data;
	}

	eDragPayloadType type;
	DataVariant data;
};

static bool g_isDragging = false;

class DragDrop
{
public:
	static DragDrop& Get()
	{
		static DragDrop instance;
		return instance;
	}

	bool DragBegin()
	{
		return ImGui::BeginDragDropSource();
	}

	void DragEnd()
	{
		ImGui::EndDragDropSource();
	}

	void DragPayload(const DragDropPayload& payload)
	{
		ImGui::SetDragDropPayload((const char*)&payload.type, (void*)&payload, sizeof(payload), ImGuiCond_Once);
	}

	DragDropPayload* GetPayload(eDragPayloadType type)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* imguiPayload = ImGui::AcceptDragDropPayload((const char*)&type))
			{
				return (DragDropPayload*)imguiPayload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		return nullptr;
	}
};