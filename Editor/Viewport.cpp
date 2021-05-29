#include "Viewport.h"


namespace Editor
{
    Viewport::Viewport(Editor* pEditor)
        : Widget(pEditor)
    {
        m_title = "Viewport";
    }
}