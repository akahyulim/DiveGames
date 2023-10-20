#pragma once

namespace Dive
{
    enum class eRenderingPath
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
        Count
    };

    enum class eDepthStencilState
    {
        DepthReadWrite,
        DepthReadWrite_StencilReadWrite,
        GBuffer,
        Count
    };

    enum class eBlendState
    {
        Addictive,
        Count
    };

    enum class eConstantBuffer
    {
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