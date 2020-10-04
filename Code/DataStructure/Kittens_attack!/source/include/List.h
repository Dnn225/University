#pragma once

class List
{
public:
	// �� ���� "������ ��������� �������" �������� �������� wchar_t, �.�. �������� ������� ������� �������
	using data_t = wchar_t;
	
	// ������� �������� value �� ������� index
	void InsertByIndex(data_t value, int index);

	// �������� �������� �� ������� index 
	void RemoveByIndex(int index);

	// ������� ����� ����������
	void Clear();

	// ���������� ������ ������
	int Size() const;

	// ���������� ������ �������� �������� data
	// ���� ������� �� ������, �� ������������ -1
	int GetPosition(data_t data) const;

	// ���������� ������� �� ���������� �������.
	// �� ������� �� �����, �� ���-�� ��-���� ����� ����
	// ������ ���� ������, ����� ������� ��� � �������
	data_t Get(int index) const { return m_data[index]; }

private:
	vector<data_t> m_data;
};