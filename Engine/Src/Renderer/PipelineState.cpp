#include "divepch.h"
#include "PipelineState.h"

namespace Dive
{
    bool PipelineState::IsValid()
    {
        // shader가 존재하느냐?

        // state도 다 있어야 하고, primitive_topology는 unknown이면 false

        // 위의 설정 조합으로 판단

        return true;
    }
}