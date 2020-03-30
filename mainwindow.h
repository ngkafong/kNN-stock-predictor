#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QComboBox>
#include <QtCharts>
#include <QChartView>
#include <QFile>
#include <QVector>

#include <QByteArray>

#include "listmanager.h"
#include "datamining.h"
#include "get_data.h"

using namespace QtCharts;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_download_clicked();

    void on_output_screen_textChanged();

    void on_button_generate_clicked();

    void on_check_manual_input_clicked();

    void on_button_submit_clicked();

    void on_button_clear_clicked();

    void on_button_chart_record_clicked();

    void on_button_chart_result_clicked();

private:
    Ui::MainWindow *ui;
    ListManager *List;

    // FileDownloader Pointer
    FileDownloader* download;

    // Data Mining Pointer
    Dataminer* mdata;
    QVector<double> result;

    // Chart Data Members
    QLineSeries* prev_series;
    QChart* prev_chart;
    bool update_property = false;

    // Process Status Data Members
    bool model_generated = false;
    bool manual_checked = false;

    // UI Display Functions
    void update_output_screen(int condition);
    void update_chart_display();
    void update_chart_display(QVector<double> result, int vector_size);

};

#endif // MAINWINDOW_H
