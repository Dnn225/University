#pragma once

class SyncManager;
class ISyncStrategy;
class ThreadManagerImpl;

class ThreadSharedData
{
public:
	ThreadSharedData()
		: m_data(0), m_delta(1) {}

	void Initialize(weak_ptr<ISyncStrategy> sync, int delta = 1)
	{
		m_delta = delta; 
		m_sync = sync;
	}

	void IncreaseNumber() { m_data += m_delta; }
	void DecreaseNumber() { m_data -= m_delta; }
	int GetNumber() const { return m_data; }

	shared_ptr<ISyncStrategy> GetSyncStrategy() const { return m_sync.lock(); }

private:
	volatile int				m_data;
	weak_ptr<ISyncStrategy>		m_sync;
	int							m_delta;
};
using ThreadSharedDataPtr = shared_ptr<ThreadSharedData>;

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();
	
	// ��� �������, ����������� �� ���� �����, ������� ����������� ���� �� �������, 
	// � ������������ ��� ����-��, ��������, � ���� ������
	using LogCallback = std::function<void(int)>;

	// ��� �������, ��������� LogCallback � ����������� �� ����������� ������ ������
	// ��������, ����� ������� ������ ����� ��������� � ���� ����, � ����� ������� - � ������
	using LogCallbackFactory = std::function<LogCallback(int)>;

	// �������������� ��������, ����� ���� ����� �������� � ��������
	void Initialize(LogCallbackFactory logCallbackFactory);

	void SetSharedData(ThreadSharedDataPtr data);
	ThreadSharedDataPtr GetSharedData() const { return m_sharedData; }

	void CreateThreads();

	// �������� ���������� ���� �������
	void StartThreads();

	// ��� ���������� ������ ������� � ������� ���������� �������
	// true - ���� ������ ��������� ������ �� ��������� ���������� �������
	bool WaitThreads(int milliseconds);

	void StopThreads();

	// �������� ������������� ������
	void TerminateThreads();

	// ���������, �������� �� ������ �� ��� ���
	bool ThreadsInProcess() const;

	void SetErrorCallback(std::function<void(const string &)> callback);

private:
	struct ThreadPrivateData
	{
		ThreadPrivateData() = default;

		ThreadPrivateData(LogCallback log)
			: logFunction(log), needStop(false) {}

		LogCallback			logFunction;
		std::atomic_bool	needStop;
	};
	using ThreadPrivateDataPtr = shared_ptr<ThreadPrivateData>;

	// ��� �������, ������� ����� ����������� �������
	using ThreadFunction = std::function<void()>;

	// ������ ������� ��� ������, ������� ����� ����� ������ ������ data, � ����� ����� m_sharedData
	ThreadFunction CreateThreadFunction(ThreadPrivateDataPtr data) const;

	ThreadManagerImpl *					m_impl;

	vector<ThreadPrivateDataPtr>		m_privateData;
	ThreadSharedDataPtr					m_sharedData;

	bool								m_isInitialized;
};