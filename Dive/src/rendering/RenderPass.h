#pragma once

namespace Dive
{
	class Camera;

	class RenderPass
	{
	public:
		RenderPass() = default;
		virtual ~RenderPass() = default;

		virtual void Execute(ID3D11DeviceContext* deviceContext, const Camera* camera) = 0;
	};

	class TestPass : public RenderPass
	{
	public:
		void Execute(ID3D11DeviceContext* deviceContext, const Camera* camera) override;

	private:
	};
}
