#pragma once

namespace Dive
{
    enum class eRenderPath
    {
        Forward,
        Deferred
    };

    enum class eRendererGameObject
    {
        Geometry,
        GeometryTransparent,
        SkinnedGeometry,
        SkinnedGeometryTransparent,
        Light,
        Camera,
    };

    enum class eRasterizerState
    {
        FillSolid_CullBack,
        FillSolid_CullNone,
        ShadowGen,
        NoDepthClipFront,
        Total
    };

    enum class eBlendState
    {
        Additive,
        Transparent,
        Total
    };

    enum class eRenderTarget
    {
        GBuffer_DepthStencil,
        GBuffer_Color_SpecIntensity,
        GBuffer_Normal,
        GBuffer_SpecPower,
        FrameRender,
        FrameOutput,
        Total
    };
}