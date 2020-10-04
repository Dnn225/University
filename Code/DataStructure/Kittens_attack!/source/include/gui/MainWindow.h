#pragma once

#include "ui_MainWindow.h"
#include "Stack.h"
#include "List.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * parent = nullptr);

	void Initialize(const string & filename);
	
private slots:
	// �������-�����, � ������� �������� ������� �� ���:
	// ��� ������� �� "��������..."
	void InsertToBegin();
	void InsertByIndex();
	void InsertToEnd();

	// ��� ������� �� "�������..."
	void RemoveFromBegin();
	void RemoveByIndex();
	void RemoveFromEnd();

	// ��� ������� �� "��������"
	// ��������� ��������������� ������� ���� � ������� �������
	void Clear();

	// ��� ������� �� "����� �������"
	// ��������� ��������������� ������� ���� � ������� �������
	void GetPosition();

	// ��� ������� �� "�����" -> "������� ���������..." -> "������"
	void OnChooseList();

	// ��� ������� �� "�����" -> "������� ���������..." -> "����"
	void OnChooseStack();

	// ��� ������� �� "�����" -> "������ �� ������"
	void OnHelpRequest();

	// ��� ������������ ������� �� �����������
	void OnInactionTimer();
private:
	// ������������ ��������� ����� � ���� ����� (��� �������) � ��������������� ��� ���������� ���������� ��� ������
	// ���� ������ �� ������������� ���������� ����, �� ������������ false
	bool GetDataForList(const QString & input, List::data_t & out) const;
	bool GetDataForStack(const QString & input, Stack::data_t & out) const;

	// ���������� ������ �������� ��� ��������, ������� ��� ������������
	// ���� ������������ �� ��� ��������, ������������ -1
	int GetEditRemoveIndex() const;
	
	// ���������� ������ �������� ��� �������, ������� ��� ������������
	// ���� ������������ �� ��� ��������, ������������ -1
	int GetEditInsertIndex() const;

	// ��� ������� ������� �������� � ��������� ���������
	// ��������� ������������ �������� � ��������� ��������������� ������� ����
	void _InsertByIndex(int index);

	// ��� ������� �������� �������� �� ���������� ����������
	// ��������� ������������ �������� � ��������� ��������������� ������� ����
	void _RemoveByIndex(int index);
	
	Ui::MainWindow		ui;
	bool				m_isListChosen = true;
	Stack				m_stack;
	List				m_list;
	
	QPointer<QTimer>	m_inactionTimer;
	int					m_inactionCounter = 0; // ������� ������������ ������ ������� �� �����������
	
	bool				m_helpOpennedOnce = false;
};
