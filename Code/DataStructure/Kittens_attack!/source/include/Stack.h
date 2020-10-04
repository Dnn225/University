#pragma once

class Stack
{
public:
	using data_t = float;
	
	// ����� ������� value �� ������� �����
	void Push(data_t value);

	// ������� ������� � ������� �����
	void Pop();

	// ���������� �������, ������� ����� �� ������� �����
	data_t Top() const;

	// ������� ����
	void Clear();

	// ���������� ���������� ��������� � �����
	int Size() const;

private:
	std::forward_list<data_t>	m_data;
	int							m_size = 0;
};