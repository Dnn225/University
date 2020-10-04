#include "stdafx.h"

#include "GeneratorId.h"

int GeneratorId::Next()
{
	unsigned id;
	// ����� �������� ��������� ����� [0; m_maximum]
	std::uniform_int_distribution<unsigned> dist(0, m_maximum);
	while (true)
	{
		// ��������� ��������� ���������� ��������� ����� � ��������� �������� [0; 100]
		id = dist(m_randomGenerator);

		// ���� ���� ��� � ����������, ������ ��������� ��� � ���������
		auto it = m_ids.find(id);
		if (it == m_ids.end())
		{
			m_ids.insert(id);
			return id;
		}
	}
}

void GeneratorId::Free(unsigned id)
{
	m_ids.erase(id);
}
