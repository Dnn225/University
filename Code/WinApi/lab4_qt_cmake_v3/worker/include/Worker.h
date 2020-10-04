#pragma once

#include "ipc.h"

class ClientWorker
{
public:
	using UpdateTTLCallback = std::function<void(int)>;
	using LogCallback = std::function<void(const string &)>;

	~ClientWorker();

	// receiver - ���� �������, �������� ���������� ���������
	bool Initialize(int workerId, int ttl, ipc::Receiver receiver, LogCallback onLog, UpdateTTLCallback onTick);

	void ForceStopWork();
	bool IsWorkCompleted() const { return m_workIsCompleted; }

private:
	// �������� ���������������� �������� ������
	bool Run();

	// ��������� ��� ����������� �������, ����� ��� ������ ���� ������� �� �����
	// newReceiver - ����� ���� �������
	bool SendChangeReceiver(ipc::Receiver newReceiver);
	
	// ��������� ��� ����������� �������, ��� ������ �������� ������
	bool SendWorkCompleted();

	// ������������� ��������� �� �������
	void LauncherListen();

	void LogMessage(const string & message);

	bool				m_isInitialized {false};
	int					m_workerId {0};
	int					m_ttl {0};
	ipc::Receiver		m_receiver;

	UpdateTTLCallback	m_ttlUpdateCallback;
	LogCallback			m_logger;

	std::atomic_bool	m_workIsCompleted {false};
	std::atomic_bool	m_forseStop {false};
	std::thread			m_mainLoopThread;
};