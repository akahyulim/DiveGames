#include "divepch.h"
#include "PipelineState.h"

namespace Dive
{
    PipelineState& PipelineState::operator=(const PipelineState& ref)
    {
        this->primitiveTopology     = ref.primitiveTopology;
        this->pInputLayout          = ref.pInputLayout;

        this->pVertexShader         = ref.pVertexShader;

        this->pRasterizerState      = ref.pRasterizerState;
        this->pViewport             = ref.pViewport;

        this->pPixelShader          = ref.pPixelShader;

        this->pBlendState           = ref.pBlendState;
        this->pDepthStencilState    = ref.pDepthStencilState;
        this->pDepthStencilView     = ref.pDepthStencilView;
        this->renderTargetViews     = ref.renderTargetViews;

        return *this;
    }

    bool PipelineState::operator==(const PipelineState& ref)
    {
        if ((this->primitiveTopology != ref.primitiveTopology) &&
            (this->pInputLayout != ref.pInputLayout) &&
            (this->pVertexShader != ref.pVertexShader) &&
            (this->pRasterizerState != ref.pRasterizerState) &&
            (this->pViewport != ref.pViewport) &&
            (this->pPixelShader != ref.pPixelShader) &&
            (this->pBlendState != ref.pBlendState) &&
            (this->pDepthStencilState != ref.pDepthStencilState) &&
            (this->pDepthStencilView != ref.pDepthStencilView) &&
            (this->renderTargetViews != ref.renderTargetViews))
            return false;

        return true;
    }
    
    bool PipelineState::IsValid()
    {
        // shader가 존재하느냐?

        // state도 다 있어야 하고, primitive_topology는 unknown이면 false

        // 위의 설정 조합으로 판단

        return true;
    }
}