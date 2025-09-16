#pragma once

namespace Dive
{
	class Camera;

	class RenderPass
	{
	public:
		RenderPass() = default;
		virtual ~RenderPass() = default;

		virtual void Execute(const Camera* cameraCom) = 0;
	};

	class ForwardBase : public RenderPass
	{
	public:
		void Execute(const Camera* cameraCom) override;

	private:
	};

	class ForwardAdd : public RenderPass
	{
	public:
		void Execute(const Camera* cameraCom) override;

	private:
	};

	class Transparent : public RenderPass
	{
	public:
		void Execute(const Camera* cameraCom) override;

	private:
	};

	class ResolveScene : public RenderPass
	{
	public:
		void Execute(const Camera* cameraCom) override;
	};
}
