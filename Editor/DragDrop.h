#pragma once
#include "External/ImGui/imgui.h"
#include <variant>

namespace DiveEditor
{
	enum class eDragPayloadType
	{
		Unknown,
		GameObject,
		Texture,
		Model,
	};

	struct DragDropPayload
	{
		using DataVariant = std::variant<const char*, unsigned int>;

		DragDropPayload(eDragPayloadType type = eDragPayloadType::Unknown, DataVariant data = nullptr)
		{
			this->type = type;
			this->data = data;
		}

		eDragPayloadType type;
		DataVariant data;
	};

	static bool g_bDragging = false;

	class DragDrop
	{
	public:
		static DragDrop& GetInstance()
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
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((const char*)&type))
				{
					return (DragDropPayload*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			return nullptr;
		}

	private:
		DragDrop() = default;
	};
}