#include "pch.h"

#include "gui/MainWindow.h"
#include "gui/CatReaction.h"

#include <QDebug>
#include <QTimer>
#include <QMessageBox>

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/icon.png"));

	m_inactionTimer = new QTimer(this);
	m_inactionTimer->setInterval(15'000); // 10 ������
	connect(m_inactionTimer, &QTimer::timeout, this, &MainWindow::OnInactionTimer);
	connect(ui.helpRequest, &QAction::triggered, this, &MainWindow::OnHelpRequest);
		
	connect(ui.chooseList, &QAction::triggered, this, &MainWindow::OnChooseList);
	connect(ui.chooseStack, &QAction::triggered, this, &MainWindow::OnChooseStack);
	
	connect(ui.insertBegin, &QPushButton::clicked, this, &MainWindow::InsertToBegin);
	connect(ui.insertIndex, &QPushButton::clicked, this, &MainWindow::InsertByIndex);
	connect(ui.insertEnd, &QPushButton::clicked, this, &MainWindow::InsertToEnd);
	
	connect(ui.removeBegin, &QPushButton::clicked, this, &MainWindow::RemoveFromBegin);
	connect(ui.removeIndex, &QPushButton::clicked, this, &MainWindow::RemoveByIndex);
	connect(ui.removeEnd, &QPushButton::clicked, this, &MainWindow::RemoveFromEnd);
	connect(ui.clearContainer, &QPushButton::clicked, this, &MainWindow::Clear);

	connect(ui.searchValue, &QPushButton::clicked, this, &MainWindow::GetPosition);
}

void MainWindow::Initialize(const string& filename)
{
	ui.catHelper->Load(filename);
	
	// �� ��������� ������������� ��������� ����������� - ������, - � ����������� ��� ����
	ui.chooseList->trigger();
	ui.catHelper->SetReaction(CatReaction::Event::FIRST_MEETING);
	
	// ��������� ������, ������� �������� ������� �����������
	m_inactionTimer->start();
}

void MainWindow::InsertToBegin()
{
	_InsertByIndex(0);
}

void MainWindow::InsertByIndex()
{
	_InsertByIndex(GetEditInsertIndex());
}

void MainWindow::InsertToEnd()
{
	int lastIndex;
	if (m_isListChosen)
	{
		lastIndex = m_list.Size();
	}
	else
	{
		lastIndex = m_stack.Size();
	}

	_InsertByIndex(lastIndex);
}

void MainWindow::RemoveFromBegin()
{
	_RemoveByIndex(0);
}

void MainWindow::RemoveByIndex()
{
	_RemoveByIndex(GetEditRemoveIndex());
}

void MainWindow::RemoveFromEnd()
{
	int lastIndex;
	if (m_isListChosen)
	{
		lastIndex = m_list.Size();
	}
	else
	{
		lastIndex = m_stack.Size();
	}

	_RemoveByIndex(lastIndex - 1);
}

void MainWindow::Clear()
{
	// ������������� ������ ������� ��� ����� ��������
	m_inactionTimer->start();
	m_inactionCounter = 0;
	
	if (m_isListChosen)
	{
		m_list.Clear();
	}
	else
	{
		m_stack.Clear();
	}
	ui.table->clear();
}

void MainWindow::GetPosition()
{
	// ������������� ������ ������� ��� ����� ��������
	m_inactionTimer->start();
	m_inactionCounter = 0;
	
	CatReaction reaction;
	
	// ������� ���� ��� ���������� (����� "������� ������..." ��� "������� �� ������")
	ui.searchResult->clear();

	// �������� ��������, ������� ����� �����, �������� �������������
	QString valueToSearchFromEdit = ui.editSearchValue->text();
	if (m_isListChosen)
	{
		List::data_t data;
		if (GetDataForList(valueToSearchFromEdit, data))
		{
			int pos = m_list.GetPosition(data);
			if (pos != -1)
			{
				qDebug() << "List: Element was successfully found!";
				
				ui.searchResult->setText(QString::fromStdWString(L"������ �������� �������� %1.").arg(pos));
				
				reaction = CatReaction(
					CatReaction::ContainerType::LIST,
					CatReaction::Event::SEACH,
					CatReaction::Result::SUCCESSED
				);
			}
			else
			{
				ui.searchResult->setText(QString::fromStdWString(L"������� �� ������."));
				
				qDebug() << "List: Element was not found!";
				
				reaction = CatReaction(
					CatReaction::ContainerType::LIST,
					CatReaction::Event::SEACH,
					CatReaction::Result::FAILED
				);
			}
		}
		else
		{
			qDebug() << "List: Trying to find wrong element.";
			reaction = CatReaction(
				CatReaction::ContainerType::LIST,
				CatReaction::Event::INVALID_DATA
			);
		}
	}
	else
	{
		Stack::data_t data;
		if (GetDataForStack(valueToSearchFromEdit, data))
		{
			if (m_stack.Top() == data)
			{
				ui.searchResult->setText(QString::fromStdWString(L"������� ������� �� ������� �����!"));
				
				qDebug() << "Stack: Element was found on top!";

				reaction = CatReaction(
					CatReaction::ContainerType::STACK,
					CatReaction::Event::SEACH,
					CatReaction::Result::SUCCESSED
				);
			}
			else
			{
				ui.searchResult->setText(QString::fromStdWString(L"������� ������� �� �� ������� �����!"));
				
				qDebug() << "Stack: Element was not found on top! Cannot check other elements in stack!";
				
				reaction = CatReaction(
					CatReaction::ContainerType::STACK,
					CatReaction::Event::SEACH,
					CatReaction::Result::IMPOSIBLE
				);
			}
		}
		else
		{
			qDebug() << "Stack: Trying to find wrong data.";
			
			reaction = CatReaction(
				CatReaction::ContainerType::STACK,
				CatReaction::Event::INVALID_DATA
			);
		}
	}

	ui.catHelper->SetReaction(reaction);
}

void MainWindow::OnChooseList()
{
	// ������������� ������ ������� ��� ����� ��������
	m_inactionTimer->start();
	m_inactionCounter = 0;
	
	m_isListChosen = true;
	ui.groupBox->setTitle(QString::fromStdWString(L"������"));
	ui.chooseList->setChecked(true);
	ui.chooseStack->setChecked(false);

	// �������������� ���������� �������, � ������� ����� ���� �������� ���������� ������
	ui.table->setRowCount(0);
	ui.table->setRowCount(m_list.Size());
	for (int i = 0; i < m_list.Size(); ++i)
	{
		// ������ ������ ������ � ����� ������ - ������ ��������
		auto item = new QTableWidgetItem(QString("%1").arg(i));
		ui.table->setItem(i, 0, item);
		item->setTextAlignment(Qt::AlignCenter);
		
		// ������ ������ ������ � ����� ������ - �������� �������� �� ������ �� ������� i	
		auto value = m_list.Get(i);
		item = new QTableWidgetItem(QString::fromStdWString(std::wstring(1, value)));
		ui.table->setItem(i, 1, item);
		item->setTextAlignment(Qt::AlignCenter);
	}
}

void MainWindow::OnChooseStack()
{
	// ������������� ������ � ������ ��� ����� ��������
	m_inactionTimer->start();
	m_inactionCounter = 0;
	
	m_isListChosen = false;
	ui.groupBox->setTitle(QString::fromStdWString(L"����"));
	ui.chooseList->setChecked(false);
	ui.chooseStack->setChecked(true);

	// �������������� ���������� �������, � ������� ����� ���� �������� ���������� ������
	ui.table->setRowCount(0);
	ui.table->setRowCount(m_stack.Size());

	// �������� ����, ����� ����� ����� ���� ���������� �� ���� ���������� �������
	auto temp = m_stack;

	// 1. ������� � ����� ����� ������� �������
	// 2. ����� ��� � ������ �������
	int i = 0;
	while (temp.Size() > 0)
	{
		// ������ ������ ������ ��� ����� ������ � ���������� ���� ������, ���������� ��� � �����
		auto item = new QTableWidgetItem(QString("%1").arg(m_stack.Size() - i - 1));
		ui.table->setItem(i, 0, item);			
		item->setTextAlignment(Qt::AlignCenter);
		
		// ������ ������ ������ � ����� ������ - �������� � ������� �����
		auto topValue = temp.Top();
		temp.Pop();		
		item = new QTableWidgetItem(QString("%1").arg(topValue));
		ui.table->setItem(i, 1, item);
		item->setTextAlignment(Qt::AlignCenter);
		++i;
	}
}

void MainWindow::OnHelpRequest()
{	
	m_inactionTimer->stop();
	
	if (m_helpOpennedOnce)
	{
		ui.catHelper->SetReaction(CatReaction::Event::HELP_READING_START_N);
	}
	else
	{
		ui.catHelper->SetReaction(CatReaction::Event::HELP_READING_START);
	}
	
	
	auto text =	
L"���� (����. stack � ������; �������� ����) � ����������� ��� ������, �������������� ����� ������ ���������, �������������� �� �������� LIFO (����. last in � first out, ���������� ������ � ������ �����).\n\
���� ������� �� �����(� ������� � ��� �����), ������� ������������ � ���� ���������, ���������� �����-���� ������ � ��������� ���� ������ ��������� �� ��������� �������. ��� �����, �� ������� �������� ������ � ��������� ���������. \n\
� ����� ��������� ����������� ����������� ������� ��� ������ � ���:\n\
	1. Push - �������� � ���������;\n\
	2. Pop - ������� �� ����������;\n\
	3. Top - �����������/������� ������� �� ����������;\n";
	
	auto helpDialog = QMessageBox(QMessageBox::Information, 
		QString::fromStdWString(L"������� �� ������"), QString::fromStdWString(text), QMessageBox::StandardButton::Ok, this);
	helpDialog.exec();
	
	ui.catHelper->SetReaction(CatReaction(CatReaction::Event::HELP_READING_END));
	
	m_inactionTimer->start();
	
	m_helpOpennedOnce = true;
}

int MainWindow::GetEditRemoveIndex() const
{
	bool ok = false;
	int index = ui.editRemoveIndex->text().toInt(&ok);
	if (ok)
	{
		return index;
	}
	return -1;
}

int MainWindow::GetEditInsertIndex() const
{
	bool ok = false;
	int index = ui.editInsertIndex->text().toInt(&ok);
	if (ok)
	{
		return index;
	}
	return -1;
}

bool MainWindow::GetDataForList(const QString & input, List::data_t & out) const
{
	if (input.isEmpty())
	{
		return false;
	}

	auto str = input.toStdU32String();
	if (str.size() != 1)
	{
		return false;
	}
	
	out = str.back();
	return true;
}

bool MainWindow::GetDataForStack(const QString & input, Stack::data_t & out) const
{
	if (input.isEmpty())
	{
		return false;
	}

	bool isDouble = false;
	float value = input.toDouble(&isDouble);
	if (!isDouble)
	{
		return false;
	}
	
	out = value;
	return true;
}

void MainWindow::_InsertByIndex(int index)
{
	// ������������� ������ ������� ��� ����� ��������
	m_inactionTimer->start();
	m_inactionCounter = 0;
	
	QString insertValueFromEdit = ui.editInsertValue->text();
	CatReaction reaction;
	
	if (m_isListChosen)
	{
		List::data_t data;
		if (GetDataForList(insertValueFromEdit, data))
		{			
			if (index >= 0 && index <= m_list.Size())
			{
				qDebug() << "List: Inserted successfully!";
				
				m_list.InsertByIndex(data, index);

				// ��������� ����� ������ ������ � �������
				ui.table->insertRow(index);
				
				// ������ ������ ������ � ����� ������ - ������ ����� ������
				auto item = new QTableWidgetItem(QString("%1").arg(index));			
				item->setTextAlignment(Qt::AlignCenter);
				ui.table->setItem(index, 0, item);
				
				// ������ ������ ������ � ����� ������ - ��������, �������� �������������
				item = new QTableWidgetItem(QString::fromStdWString(std::wstring(1, data)));
				item->setTextAlignment(Qt::AlignCenter);
				ui.table->setItem(index, 1, item);

				// ��������� ��� ������ ����������� � �������, ���������� � ��� ������ (������ ������) �� 1 �������
				for (int i = index + 1; i < m_list.Size(); ++i)
				{
					ui.table->item(i, 0)->setText(QString("%1").arg(i));
				}
				
				reaction = CatReaction(
					CatReaction::ContainerType::LIST,
					CatReaction::Event::INSERTION,
					CatReaction::Result::SUCCESSED
				);
			}
			else
			{
				qDebug() << "List: Failed insert! Wrong index.";
				reaction = CatReaction(
					CatReaction::ContainerType::LIST,
					CatReaction::Event::OUT_OF_RANGE_INDEX
				);
			}
		}
		else
		{
			qDebug() << "List: Failed to insert! Wrong data.";
		
			reaction = CatReaction(
				CatReaction::ContainerType::LIST,
				CatReaction::Event::INVALID_DATA
			);
		}
	}
	else
	{
		Stack::data_t data;
		if (GetDataForStack(insertValueFromEdit, data))
		{
			if (index == m_stack.Size())
			{
				m_stack.Push(data);

				// ��������� ������ � �������:
				ui.table->insertRow(0);
				
				// ������ ������ ������ � ����� ������ - ������ ����� ������
				auto item = new QTableWidgetItem(QString("%1").arg(m_stack.Size() - 1));			
				item->setTextAlignment(Qt::AlignCenter);
				ui.table->setItem(0, 0, item);
				
				// ������ ������ ������ � ����� ������ - ���������� �� ������������ ��������
				item = new QTableWidgetItem(QString("%1").arg(data));			
				item->setTextAlignment(Qt::AlignCenter);
				ui.table->setItem(0, 1, item);

				// � ����� �������� ����������� ������ ��� �������������, �.�. �� �� �� ������ - ������� ������ ����������� � �����
				
				qDebug() << "Stack: Inserted successfully!";
				
				reaction = CatReaction(
					CatReaction::ContainerType::STACK,
					CatReaction::Event::INSERTION,
					CatReaction::Result::SUCCESSED
				);
			}
			else
			{
				qDebug() << "Stack: Failed to insert! Index is not last.";				
				reaction = CatReaction(
					CatReaction::ContainerType::STACK,
					CatReaction::Event::INSERTION,
					CatReaction::Result::IMPOSIBLE
				);
			}
		}
		else
		{
			qDebug() << "Stack: Failed to insert! Wrong data.";
			reaction = CatReaction(
				CatReaction::ContainerType::STACK,
				CatReaction::Event::INVALID_DATA
			);
		}
	}
	
	ui.catHelper->SetReaction(reaction);
}

void MainWindow::_RemoveByIndex(int index)
{
	// ������������� ������ ������� ��� ����� ��������
	m_inactionTimer->start();
	m_inactionCounter = 0;
	
	CatReaction reaction;
	
	if (m_isListChosen)
	{
		if (index >= 0 && index < m_list.Size())
		{
			m_list.RemoveByIndex(index);
			
			ui.table->removeRow(index);

			// ��������� ��� ������ ����������� � �������, ������������� �� ����� �������� �������
			for (int i = index; i < m_list.Size(); ++i)
			{
				ui.table->item(i, 0)->setText(QString("%1").arg(i));
			}
			
			qDebug() << "List: Removed successfully!";
			
			reaction = CatReaction(
				CatReaction::ContainerType::LIST,
				CatReaction::Event::REMOVING,
				CatReaction::Result::SUCCESSED
			);
		}
		else
		{
			reaction = CatReaction(
				CatReaction::ContainerType::LIST,
				CatReaction::Event::OUT_OF_RANGE_INDEX
			);
			qDebug() << "List: Failed to remove! Wrong index.";	
		}
	}
	else
	{
		// �������� �� ����� ����������� ������ ����
		// 1. ���� �� ����
		// 2. ������ ���������� �������� - ��� ������ ���������� ��������
		if (m_stack.Size() > 0 && index == m_stack.Size() - 1)
		{
			m_stack.Pop();
			
			// ��� ����� ������ ������� ������ ������, �.�. ������ ��� �������� �����
			ui.table->removeRow(0);

			// ��� �����, ����� ��, ��� ������������� ��������� ������� � ����������� ������� �������
			
			qDebug() << "Stack: Removed successfully!";
			
			reaction = CatReaction(
				CatReaction::ContainerType::STACK,
				CatReaction::Event::REMOVING,
				CatReaction::Result::SUCCESSED
			);
		}
		else
		{
			qDebug() << "Stack: Failed to remove! Index is not last.";
			
			reaction = CatReaction(
				CatReaction::ContainerType::STACK,
				CatReaction::Event::REMOVING,
				CatReaction::Result::IMPOSIBLE
			);
		}
	}
	ui.catHelper->SetReaction(reaction);
}

void MainWindow::OnInactionTimer()
{
	++m_inactionCounter;

	CatReaction reaction;
	
	// ���� ����������� ������ ����� ������
	if (m_inactionCounter * m_inactionTimer->interval() / 1000 > 60)
	{
		reaction = CatReaction(CatReaction::Event::INACTION_TIMEOUT_LONG);
	}
	else
	{
		// � ������������ 50% ��������� ���� ���� ��� ������� �� ��������� �������, ���� ������
		if (rand() % 2)
		{
			reaction = CatReaction(CatReaction::Event::INACTION_TIMEOUT_SMALL_1);	
		}
		else
		{
			reaction = CatReaction(CatReaction::Event::INACTION_TIMEOUT_SMALL_2);
		}		
	}
	
	ui.catHelper->SetReaction(reaction);
	// ������������� ������ �������
	m_inactionTimer->start();
}
