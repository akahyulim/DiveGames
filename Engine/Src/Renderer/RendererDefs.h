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
        Count
    };

    enum class eDepthStencilState
    {
        DepthReadWrite,
        DepthReadWrite_StencilReadWrite,
        GBuffer,
        DepthDiabled,   // skydome에서 off용으로...
        Count
    };

    enum class eBlendState
    {
        Addictive,
        Count
    };

    enum class eRenderTarget
    {
        GBuffer_DepthStencil,
        GBuffer_Color_SpecIntensity,
        GBuffer_Normal,
        GBuffer_SpecPower,
        FrameRender,
        FrameOutput,
        Count
    };
}