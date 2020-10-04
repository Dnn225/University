#pragma once

struct ThreadData;

class ThreadManager
{
public:
	ThreadManager();

	~ThreadManager();

	using LogFunction = std::function<void(int)>;
	using ThreadFunction = std::function<void()>;

	void CreateThreads(std::function<LogFunction(int)> logCreator, shared_ptr<ThreadData> threadData, int threadCount = 2);

	// ������� �����, ����������� � ��� �������, �� �� �������� ��� ����������
	void CreateThread(ThreadFunction func);

	// �������� ���������� ���� �������
	void StartThreads();

	// ��� ���������� ������ ������� � ������� ���������� �������
	// ������������ true ���� ������ ������� ��������� ���� ������
	// � false � ������ ������
	bool WaitThreads(int milliseconds);

	// �������� ������������� ������
	void TerminateThreads();

	bool ThreadsInProcess() const;

	void SetErrorCallback(std::function<void(const string &)> callback);

private:
	ThreadFunction CreateThreadFunction(LogFunction logger, shared_ptr<ThreadData> threadData) const;

	class ThreadManagerImpl;

	ThreadManagerImpl* m_impl;
};

class ISyncStrategy;

struct ThreadData
{
	ThreadData()
		: m_data(0), m_delta(0) {}

	volatile int				m_data;
	std::atomic_bool			m_forceStop;
	weak_ptr<ISyncStrategy>		m_sync;
	int							m_delta;
};