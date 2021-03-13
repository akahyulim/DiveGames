#pragma once

namespace Dive
{
	// �ᱹ ������ ������ ���ΰ��� ���� �� ��𿡼� ������ ���ΰ��� �ľ��ؾ� �Ѵ�.
	// ������� ��ġȭ�� ���̰�, �̵��� �̿��� �׷��� Ȥ�� ���ڿ� �������� ��� �� ������ �� �־�� �Ѵ�.

	// cpu, gpu, frame, fps ���
	// ������ ���, ����, �ִ� �� 
	// ����Ƽ�� ��� �޸� ��뷮�� �����ش�.
	// �׷��� �޸𸮵� cpu�� gpu�� �ٸ��ٵ�... �׳� �����ִ� ����� �����ΰ�?
	// �ֿ��� ���׺��� ������ �� ���� �߰��ذ��� ������ �´� �� ����.

	// wicked
	// frame, rangeCPU, rangeGPU�� ������ �����Ѵ�.
	// begin ~ end frame�� run time�� run�� ���� ���۰� ���̴�. ���ο��� cpu, gpu�� ���� �����ȴ�.
	// cpu, gpu�� runtime�Ӹ� �ƴ϶� ���� ��ü������ �����ȴ�.
	class Profiler
	{
	public:
		static Profiler& GetInstance()
		{
			static Profiler instance;
			return instance;
		}

		void SetEnabled(bool use);
		bool IsEnabled() const { return m_bEnabled; }

	private:
		Profiler() = default;
		~Profiler();

	private:
		bool m_bEnabled = false;
	};
}