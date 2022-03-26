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
        // shader�� �����ϴ���?

        // state�� �� �־�� �ϰ�, primitive_topology�� unknown�̸� false

        // ���� ���� �������� �Ǵ�

        return true;
    }
}