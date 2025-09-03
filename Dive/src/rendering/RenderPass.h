#pragma once

namespace Dive
{
	class Camera;

	class RenderPass
	{
	public:
		RenderPass() = default;
		virtual ~RenderPass() = default;

		virtual void Execute(const Camera* camera) = 0;
	};

	class ForwardBase : public RenderPass
	{
	public:
		void Execute(const Camera* camera) override;

	private:
	};

	class ForwardAdd : public RenderPass
	{
	public:
		void Execute(const Camera* camera) override;

	private:
	};
}
