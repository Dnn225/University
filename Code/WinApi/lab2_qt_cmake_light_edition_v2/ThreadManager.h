#pragma once

enum class SyncType
{
	NO_SYNCHRONIZATION,
	CRITICAL_SECTION,
	SEMAPHORE,
	MUTEX,
	EVENT
};

using LogCallback = std::function<void(int)>;

class ThreadParams;
class ThreadCommonData;

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	// Initialize ���������� �������� �� �� ����� ������ ������� ThreadManager
	void Initialize(const LogCallback & left, const LogCallback & right);

	// Configurate ���������� ��� ��������� ������� ������������� ����� ������ �������� ������� (��� ������ OnStart)
	void Configurate(int delta, SyncType type);

	// Reset ���������� ���������, ������������� ����� Configurate
	void Reset();

	// ������ � ��������� ������
	void StartThreads();

	// ���� �� ��������� ����� ������ ��������� ���� ������, �� ������������ true, ����� - false
	bool WaitThreads(int millisec) const;

	bool ThreadsInProcess() const { return !WaitThreads(0); }
	
	// ���������� ���������� �������
	void TryStopThreads();

	// ��������� ���������� �������. ���������� �� ������������.
	void ForceStopThreads();

private:
	static DWORD ThreadFunction(LPVOID params);

	struct Thread
	{
		Thread() : handle(nullptr), params(nullptr) {}

		HANDLE				handle;
		ThreadParams *		params;
	};

	Thread				m_threadLeft;
	Thread				m_threadRight;
	ThreadCommonData *	m_threadCommonData;
	LogCallback			m_callbackLeft;
	LogCallback			m_callbackRight;

	CRITICAL_SECTION		m_critSection;
	HANDLE					m_semaphore;
	HANDLE					m_mutex;
	HANDLE					m_event;

	bool				m_isInitialized;
	bool				m_isConfigurated;
};

/*	������ ThreadCommonData �������� ������� ��� ��������, ��������� � ����,
	�� ���� �� ����� ��������� � �������� ����� � ��������� ������. 
	�������������� � ������� �������� ������ ������ �� ���������. */
class ThreadCommonData
{
public:
	ThreadCommonData(int data = 0, int delta = 1)
		: m_forseStop(false), m_data(data), m_delta(delta)
	{}
	
	void IncreaseNumber() { m_data += m_delta; }
	void DecreaseNumber() { m_data -= m_delta; }
	int GetNumber() const { return m_data; }

	void SetForseStop(bool value = true) { m_forseStop = value; }
	bool NeedForseStop() const { return m_forseStop; }

private:
	std::atomic_bool	m_forseStop;
	volatile int		m_data;
	int					m_delta;
};

/*	������ ThreadParams ����� ������������ � �������, ������� ��������� ������ �����.
	��� ����� ������ ����� ����� ����� ����������� ������������ �������, ����������� 
	��� �������������.
	����� ������ ������ ����� ������ ��� ���������� �����������. � ����� ������ ���
	� ����� �������� � QListWidget, ��� ������ �� �����. */
class ThreadParams
{
public:
	ThreadParams();

	void Initialize(ThreadCommonData *data, SyncType type, void * handle, const LogCallback & callback);

	// �������, ����������� ������������� �������
	void Lock();
	void Unlock();
	void Wait();
	bool IsLocked();

	// ��������� �������, � ������� �������� ����� ����� �������� � �������
	ThreadCommonData * GetData() { return m_data; }
	ThreadCommonData const * GetData() const { return m_data; }

	// �������� ������ ��� ���������� ����������
	void DoLogData();

private:
	ThreadCommonData *		m_data;
	SyncType				m_syncType;
	LPCRITICAL_SECTION		m_critSection;
	HANDLE					m_handle;
	LogCallback				m_logCallback;
	bool					m_isInitialized;
};