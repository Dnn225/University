#pragma once

#include "ipc.h"
#include "sync.h"

class GeneratorId;

class ServerWorker
{
public:
	// receiver - ���� �������, �������� ����� �������� ���������
	ServerWorker(ipc::Receiver receiver);

	// �������� ��������� �� ��������
	bool SendTick();
	bool SendForceClose();

	void UpdateReceiver(ipc::Receiver newReceiver);

private:
	ipc::Receiver	m_receiver;
};

class Launcher
{
public:
	Launcher();
	~Launcher();

	void SetMaximumWorkers(int number);
	int GetCurrentWorkersNumber() const;

	void Tick();

	void SetLogCallback(std::function<void(const string &)> callback);
	void SetErrorCallback(std::function<void(const string &)> callback);

private:
	// ��������� ������ �������, ������� ��� ��������� id � ��������� �� 0 �� 100
	bool CreateWorker();

	/* ��� ������� �������, ����������� ��������� � ���� ��������� �������:
		MainLoop: ��� ������� m_tickEvent ��� ������������ ���� � ���������� 
			������� TICK ������� ����������� �������. ��������� ����������
			���������� ��������, ���� ����� �� ���������, �� ������ ��� ������.
		ListenWorkers: ������������ ��������, ������ �� ��� ���������.
			��� ��������� ��������� CLOSED ������� ������, �� �������� ������ ���������
			�� ������ ����������.
			��� ��������� ��������� RECEIVER_CHANGED ������ ���� ���������� � ����������
			�������. */
	void MainLoop();
	void ListenWorkers();

	int	GetRandomNumber(int from, int to);

	void LogError(const string & message);
	void LogMessage(const string & message);

	mutable sync::CriticalSection	m_criticalSection;
	mutable sync::Event				m_tickEvent;

	std::thread						m_threadOutput;
	std::thread						m_threadInput;
	std::atomic_bool				m_needForseStop { false };

	shared_ptr<GeneratorId>				m_generatorId;
	map<int, shared_ptr<ServerWorker>>	m_workers; // workerId <-> worker
	size_t								m_maxWorkers {0};

	std::mt19937					m_random;
	
	std::function<void(const string &)>		m_logCallback;
	std::function<void(const string &)>		m_errorCallback;
};
