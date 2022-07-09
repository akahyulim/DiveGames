#pragma once

namespace Dive
{
	class Engine;

	class Application
	{
	public:
		// ������. Ŀ�ǵ�������κ��� ���� �Ķ���͸� �Ľ� �� �� ������ �ʱ�ȭ���� ���� ���·� ����.
		Application();
		virtual ~Application() = default;

		// ���� �ʱ�ȭ ������ ȣ��. �� ������ ���� �Ķ���͸� ���� ����.
		virtual void Setup() {}
		
		// ���� �ʱ�ȭ ��, ���� ���� ���� �� ����.
		virtual void Start() {}
		
		// ���� ���� �� Ŭ����, ���ø����̼��� ȣ��.
		virtual void Stop() {}

		// ���� �ʱ�ȭ �� ���� ���� ����.
		int Run();
		// ���� �޽����� ���. ���� ������ �����ϰ�, ���� ���� �ڵ带 ����.
		void ErrorExit(const std::string& message = std::string());

	protected:
		// Dive ����
		std::shared_ptr<Engine> m_pEngine;
		// ���� �Ķ���� ��
		// ���ø����̼� ���� �ڵ�
		int m_ExitCode;
	};
}