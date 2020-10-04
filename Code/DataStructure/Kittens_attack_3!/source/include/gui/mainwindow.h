#pragma once

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget * parent = nullptr);

	// ������� ��� ������� �� ��������� ������ xValues � yValues
	void CreateChart(const wstring& name, const vector<double> & xValues, const vector<double> & yValues);
	
	// ���������� ��������� ������
	void ShowChart(const wstring& name);
private:
	QChartView * CreateChartImpl(const vector<double>& xValues, const vector<double>& yValues, bool logarithmicScales);
	
	map<wstring, int>	m_charts;
	Ui::MainWindow				ui;
};
