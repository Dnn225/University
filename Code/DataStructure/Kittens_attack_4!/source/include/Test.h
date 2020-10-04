#pragma once

// ������������� ���������� ����������
void InitializeTestingTools();

// ��������������� ���������� ����������
void DestroyTestingTools();

// ������ ��������� ����� � ��������� ���������
int CreateRandomNumber(int from = std::numeric_limits<int>::min(), int to = std::numeric_limits<int>::max());

// ������ ������ ��������� ����� (�� -size/2 �� +size/2) ���������� �������
vector<int> CreateDataSet(int size);

// ����������� ���������� �������� ������� �� ������� ������, �.�.
//		X - ������ ������
//		Y - ���������� ����������� ������� ������� ��� ������� ������ ��������
void CreateInsertionChartData(vector<double> & outXValues, vector<double> & outYValues, int epoch);

// ����������� ���������� �������� �������� �� ������� ������, �.�.
//		X - ������ ������
//		Y - ���������� ����������� ������� �������� ��� �������� ������ ��������
void CreateRemovingChartData(vector<double> & outXValues, vector<double> & outYValues, int epoch);

// ����������� ���������� �������� ������ �� ������� ������, �.�.
//		X - ������ ������
//		Y - ���������� ����������� ������� ������ ��� ������ ������ ��������
void CreateSearchChartData(vector<double> & outXValues, vector<double> & outYValues, int epoch);