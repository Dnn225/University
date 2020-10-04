#include "stdafx.h"
#include "ThreadManagerWinImpl.h"
#include "WinApiUtils.h"

ThreadManagerImpl::~ThreadManagerImpl()
{
	for (auto func : m_threadFunctions)
	{
		delete static_cast<std::function<void()>*>(func.paramsPtr);
	}
}

void ThreadManagerImpl::CreateThread(const std::function<void()> & userFunction)
{
	// ����� � ������������ ����� �������������� ��������� �����������:
	// 1. ��������������� ������� - ������� userFunction, ������� �� �������� �������, ������� ��������� void(), �� ����
	//	���������� � ��������� void
	// 2. ������� ������ - �������, ���������� �� ������� ����� �������� ������ ����� ����� ��������.
	//	��������� ������� ���������� WinApi - int(PVOID), �.�. ��������� void*, � ���������� int.

	WinFunc func;
	func.startFunc = [](LPVOID params) -> DWORD
	{
		// ����������� ������� ��������� (void*) � ��������� �� �� �������, ������� �� �������� �� ������������
		auto userFuncPtr = static_cast<std::function<void()>*>(params);
		if (userFuncPtr)
		{
			auto userFunc = *userFuncPtr;
			// ��������� ��� �������
			userFunc();	
		}
		// ������������ ������ ����� ���������� �������
		return 0;
	};

	// ����� ������������ ���������������� �������, �� ����� �������� � � �������� ��������� � ������� ������.
	// ��� ���������� ��� ����, ����� ����� ����������� ������������ �������
	// � ����� ���������� - � ����� ������ void(), - � �� � ���������� WinApi - int(LPVOID).

	// ��� ����� ����������� ���������� � ����� ������� � ������� ����� ���������������� �������, 
	// ������� ����� ����������� ����� ����� �������.
	auto funcPtr = new std::function<void()>(userFunction);
	// ��������� �� ���������������� ������� ����������� � ��������� �� void, ����� �������� � ������� ������.
	func.paramsPtr = static_cast<LPVOID>(funcPtr);
	
	// ������� �����, ������ �������� �� ������� �� �������������� �����, � � �������� ��������� ������� ������ ����� ���������.
	auto handle = ::CreateThread(nullptr, 0, func.startFunc, func.paramsPtr, CREATE_SUSPENDED, nullptr);
	if (handle == nullptr)
	{
		LogLastError("CreateThread");
		delete funcPtr;
	}
	else
	{
		m_threadFunctions.emplace_back(std::move(func));
		m_threadHandles.push_back(handle);	
	}
}

void ThreadManagerImpl::StartThreads()
{
	for (auto handle : m_threadHandles)
	{
		ResumeThread(handle);
	}
}

bool ThreadManagerImpl::WaitThreads(int waitingTime)
{
	using namespace std::chrono;
	bool ready = true;

	// ���������� ������ �����
	for (auto it = m_threadHandles.begin(); it != m_threadHandles.end();)
	{
		// �������� ������� �����
		auto startTimePoint = high_resolution_clock::now();

		auto handle = *it;

		// ��� ���������� ������ ���������� ���������� �������
		auto res = WaitForSingleObject(handle, waitingTime);

		// ���� ����� �������� ���� ������, �� ��������� ��� ����� � ������� �� ������
		if (res == WAIT_OBJECT_0 || res == WAIT_ABANDONED)
		{
			CloseHandle(handle);
			it = m_threadHandles.erase(it);
		}
		else // ���� ����� ��� �� �������� ���� ������
		{
			// ��������, ��� ���� �� ���� ����� ��� �� �������� ���� ������
			ready = false;
			// ���� ��������� ������ �� ����� ��������
			if (res == WAIT_FAILED)
			{
				// ��������� �
				LogLastError("WaitForSingleObject");
			}
			// ��� � ���������� ������ � ������
			++it;
		}

		// �������� ������� ������� ������ �� �������� �����
		int timeExpired = (high_resolution_clock::now() - startTimePoint).count();
		// ��������� ���������� ������� �������� ���������� ������, �.�. �� ��� ����� �� ������ �������
		waitingTime = std::max(0, waitingTime - timeExpired);
	}
	return ready;
}

void ThreadManagerImpl::TerminateThreads()
{
	// ���������� ������ �����
	for (auto it = m_threadHandles.begin(); it != m_threadHandles.end(); )
	{
		auto handle = *it;

		// �������� ������� �����
		bool success = ::TerminateThread(handle, 1);
		if (!success)
		{
			// ���� �� ����� �������� ������ ��������� ������, �� ��������
			LogLastError("TerminateThread");
			++it;
		}
		else
		{				
			// ��������� ����� � ������ ��������� ��������
			success = ::CloseHandle(handle);
			if (!success)
			{
				// �������� ������ � ��������� � ���������� ������
				LogLastError("CloseHandle");
				++it;
			}
			else
			{
				// � ������ ��������� �������� ������ ������� ��� �� ������
				it = m_threadHandles.erase(it);
			}
		}
	}
}

void ThreadManagerImpl::SetErrorCallback(std::function<void(const string &)> callback)
{
	m_errorCallback = callback;
}

void ThreadManagerImpl::LogLastError(const string & functionName) const
{
	EchoErrorMessage(WinApiUtils::ErrorCodeToString(GetLastError(), functionName));
}

void ThreadManagerImpl::EchoErrorMessage(const string & error) const
{
	if (m_errorCallback && !error.empty())
	{
		m_errorCallback(error);
	}
}
