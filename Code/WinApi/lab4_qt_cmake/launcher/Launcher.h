#pragma once

using LogCallback = std::function<void(const string &)>;

namespace WinApi
{
	// void SendMessage();
	// void GetMessage();
	// void WaitMessage();
	string GetLastMessage(const string & functionName = {});
}

class CritSection
{
public:
	CritSection();
	~CritSection();

	void Lock();
	void Unlock();

private:
	CRITICAL_SECTION	m_crit;
};

class Worker
{
public:
	Worker();
	~Worker();

	void Start();
	void Tick();
	void Stop();

	bool IsStoped();

private:

};

class Launcher : public std::enable_shared_from_this<Launcher>
{
public:
	Launcher();
	~Launcher();

	void SetMaximumWorkers(int number);
	int GetWorkersNumber() const;

	void CreateWorker();

	std::shared_ptr<std::mt19937> GetRandom() const { return m_random; }

	void SetLogCallback(const LogCallback & callback) { m_logCallback = callback; }
	const LogCallback & GetLogCallback() const { return m_logCallback; }

private:
	struct Thread
	{
		std::thread			thread;
		std::atomic_bool	isComplete {false};
	};

	void GenerateIds();
	int GetId();
	void FreeId(int);

	// ����������� ������ ��� ������������� ��������� �� ���, � ����� ����� ����������� ��������
	mutable CritSection		m_crit;

	// ������������ ���������� �������, ������� ����� �������
	int					m_maxWorkers;
	// ������ �������
	vector<Worker>		m_workers;
	// ������� ��������� � ������� id
	std::queue<int>		m_freeIds;
	std::queue<int>		m_busyIds;

	/* �����, ������� *� ��������* ����� ���������� ��������� ������� � ������ ���������:
		1. ��� ������� (�����������)
		2. ��������������� ���������� �� ������ (�����������)
		3. �������� ��������� �� ������ id (��������) */ 
	Thread				m_threadOutput;

	/* ���� ����� ����� ��������� ��������� � ����������� ������ worker-��. */
	Thread				m_threadInput;

	/* ��������� ��������� ����� */
	std::shared_ptr<std::mt19937>	m_random;

	/* ������ ��� ������ ��������� �� ������� */
	LogCallback		m_logCallback;
};