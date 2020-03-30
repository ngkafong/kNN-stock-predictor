#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    List(new ListManager),
    mdata (new Dataminer)
{
    ui->setupUi(this);

    update_output_screen(0);
    ui->graphicsView->hide();

    ui->statusbar->showMessage("Please download a record first.");
}

MainWindow::~MainWindow()
{
    delete ui;
    if (List->get_status_downloaded()) delete download;
    delete List;
    if (model_generated) delete mdata;
}

// ====================================================================================
//  Show Message Functions
// ====================================================================================

void show_warning_msg(QString title, QString msg)
{
    QMessageBox Msg_Warning;
    Msg_Warning.setWindowTitle(title);
    Msg_Warning.setIcon(QMessageBox::Warning);
    Msg_Warning.setText(msg);
    Msg_Warning.exec();
}

void show_critical_msg(QString title, QString msg)
{
    QMessageBox Msg_Warning;
    Msg_Warning.setWindowTitle(title);
    Msg_Warning.setIcon(QMessageBox::Critical);
    Msg_Warning.setText(msg);
    Msg_Warning.exec();
}

void show_info_msg(QString title, QString msg)
{
    QMessageBox Msg_Warning;
    Msg_Warning.setWindowTitle(title);
    Msg_Warning.setIcon(QMessageBox::Information);
    Msg_Warning.setText(msg);
    Msg_Warning.exec();
}

// ====================================================================================
//  UI functions
// ====================================================================================


void MainWindow::on_download_clicked()
{
    // Status Check
    if (ui->statusbar->currentMessage() == "Downloading..." || ui->statusbar->currentMessage() == "Downloading, please wait..."){
        ui->statusbar->showMessage("Downloading, please wait...");
        return;
    }
    // Input check
    if (ui->item_code->text()=="")
    {
        show_warning_msg("Warning", "Enter a stock code!");
        return;
    }

    // UI update
    update_output_screen(0);
    ui->graphicsView->hide(); // *Delete chart record when software close
    ui->label_no_data->setVisible(true);
    ui->statusbar->showMessage("Please download a record first.");
    ui->button_chart_record->setEnabled(false);
    ui->button_chart_result->setEnabled(false);

    // Require users to generate model again with new data
    // In order to operate other functions
    if (model_generated)
    {
        model_generated = false;
        ui->check_manual_input->setChecked(false);
        ui->price_today->setEnabled(false); ui->price_today->setText("");
        ui->price_day_1->setEnabled(false); ui->price_day_1->setText("");
        ui->price_day_2->setEnabled(false); ui->price_day_2->setText("");
        ui->price_day_3->setEnabled(false); ui->price_day_3->setText("");
        ui->price_day_5->setEnabled(false); ui->price_day_5->setText("");
        ui->price_day_10->setEnabled(false); ui->price_day_10->setText("");
        ui->price_day_20->setEnabled(false); ui->price_day_20->setText("");
        ui->price_day_30->setEnabled(false); ui->price_day_30->setText("");
        ui->button_submit->setEnabled(false);
    }

    // Prevent Memory Leak
    if (List->get_status_downloaded()) {delete download;}

    // Download data
    QUrl url = get_url(ui->item_code->text());
    cout << url.toString().toStdString() << endl;
    download = new FileDownloader(url, ui->statusbar, ui->output_screen);
}

void MainWindow::on_output_screen_textChanged()
{
    // Received download complete signal
    if (ui->output_screen->toPlainText() == "Please wait...")
    {
        // Open file
        QFile file("data.csv");
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << file.errorString();
        }

        // Invalid stock code -> no records downloaded
        if (file.size() < 300) // Recorded filesize 152 - Error Report
        {
            // Delete Download Record
            delete download; download = nullptr;
            List->set_status_downloaded(false); // Prevent Destructor crash
            ui->statusbar->showMessage("Please download a record first.");
            update_output_screen(0);
            show_critical_msg("Error", "You entered an invalid stock code!");
            return;
        }

        // Store downloaded data
        int day_count = 0;
        QVector <double> priceList;
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            priceList.append(line.split(',').at(3).toDouble());
            ++day_count;
        }
        file.close();
        priceList.removeFirst();
        List->add_new_item_list(ui->item_code->text(), day_count-1, priceList);
        ui->button_chart_record->setEnabled(true);

        // Display data
        update_output_screen(1);
        update_chart_display();
    }
}

void MainWindow::on_button_generate_clicked()
{
    // Checkings
    if (ui->statusbar->currentMessage() == "Downloading..." || ui->statusbar->currentMessage() == "Downloading, please wait...")
    {
        ui->statusbar->showMessage("Downloading, please wait...");
        return;
    }
    if (!List->get_status_downloaded())
    {
        ui->statusbar->showMessage("Please download a record first.");
        show_warning_msg("Warning", "Please download a record first.");
        return;
    }

    // Data Mining
    int k_value = ui->k_value->text().toInt();
    QVector<QVector <double>> arr = mdata->generateAttributes(List->get_price_list());
    mdata->input(arr[0]);
    mdata->trainSet(arr.mid(7), mdata->generateTargets(List->get_price_list()));
    QVector<double >result = mdata->getResultfromKNN(mdata->kNN(k_value));

    // Update Output Display
    ui->output_screen->setText("Result Generated");
    ui->output_screen->append("======================================================================");
    ui->output_screen->append("Stock Price Percentage Change Predictions ...");
    ui->output_screen->append("1 Day  Later: " + QString::number(result[0]*100) + "%");
    ui->output_screen->append("2 Days Later: " + QString::number(result[1]*100) + "%");
    ui->output_screen->append("3 Days Later: " + QString::number(result[2]*100) + "%");
    ui->output_screen->append("4 Days Later: " + QString::number(result[3]*100) + "%");
    ui->output_screen->append("5 Days Later: " + QString::number(result[4]*100) + "%");
    ui->output_screen->append("6 Days Later: " + QString::number(result[5]*100) + "%");
    ui->output_screen->append("7 Days Later: " + QString::number(result[6]*100) + "%");
    ui->output_screen->append("\n");
    update_output_screen(2);
    ui->output_screen->verticalScrollBar()->setValue(0);

    // Update Chart Display
    for (int i=0; i<7; result[i] = result[i]*100, ++i) {} // *Display in percentage
    update_chart_display(result, 7);
    this->result = result;
    ui->button_chart_result->setEnabled(true);

    // Status Update
    model_generated = true;
}

void MainWindow::on_check_manual_input_clicked()
{
    // Model Existence Check
    ui->check_manual_input->setChecked(false);
    if (!model_generated) { show_critical_msg("Error","You have to generate a model first!"); return; }

    // Toggle status
    if (manual_checked)
    {
        // Disable Manual Input
        ui->price_today->setEnabled(false); ui->price_today->setText("");
        ui->price_day_1->setEnabled(false); ui->price_day_1->setText("");
        ui->price_day_2->setEnabled(false); ui->price_day_2->setText("");
        ui->price_day_3->setEnabled(false); ui->price_day_3->setText("");
        ui->price_day_5->setEnabled(false); ui->price_day_5->setText("");
        ui->price_day_10->setEnabled(false); ui->price_day_10->setText("");
        ui->price_day_20->setEnabled(false); ui->price_day_20->setText("");
        ui->price_day_30->setEnabled(false); ui->price_day_30->setText("");
        ui->button_submit->setEnabled(false);
        manual_checked = false;
    }
    else
    {
        // Enable Manual Input
        ui->price_today->setEnabled(true);
        ui->price_day_1->setEnabled(true);
        ui->price_day_2->setEnabled(true);
        ui->price_day_3->setEnabled(true);
        ui->price_day_5->setEnabled(true);
        ui->price_day_10->setEnabled(true);
        ui->price_day_20->setEnabled(true);
        ui->price_day_30->setEnabled(true);
        ui->button_submit->setEnabled(true);
        manual_checked = true;
        ui->check_manual_input->setChecked(true);
    }

}

void MainWindow::on_button_submit_clicked()
{
    // Collect data, and price into float array
    double price_input[8];

    // Data entry w/ input type check
    bool conversion_check = true;
    price_input[0] = ui->price_today->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[1] = ui->price_day_1->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[2] = ui->price_day_2->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[3] = ui->price_day_3->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[4] = ui->price_day_5->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[5] = ui->price_day_10->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[6] = ui->price_day_20->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
    price_input[7] = ui->price_day_30->text().toDouble(&conversion_check);
    if (!conversion_check) { show_warning_msg("Warning", "You entered non-double number(s)!"); return; }
     // check passed

    // double array -> QVector<double>
    QVector<double> mdata_input;
    mdata_input.append(price_input[0]);
    for (int i=0; i<7; ++i)
    {
        mdata_input.append( (price_input[i+1]-price_input[0])/price_input[0] );
    }

    // Data Mining
    int k_value = ui->k_value->text().toInt();
    mdata->input(mdata_input);
    QVector<double >result = mdata->getResultfromKNN(mdata->kNN(k_value));

    // Update Output Display
    ui->output_screen->setText("Result Generated");
    ui->output_screen->append("======================================================================");
    ui->output_screen->append("Manual Input - Stock Price Percentage Change Predictions ...");
    ui->output_screen->append("1 Day  Later: " + QString::number(result[0]*100) + "%");
    ui->output_screen->append("2 Days Later: " + QString::number(result[1]*100) + "%");
    ui->output_screen->append("3 Days Later: " + QString::number(result[2]*100) + "%");
    ui->output_screen->append("4 Days Later: " + QString::number(result[3]*100) + "%");
    ui->output_screen->append("5 Days Later: " + QString::number(result[4]*100) + "%");
    ui->output_screen->append("6 Days Later: " + QString::number(result[5]*100) + "%");
    ui->output_screen->append("7 Days Later: " + QString::number(result[6]*100) + "%");
    ui->output_screen->append("\n");
    update_output_screen(2);
    ui->output_screen->verticalScrollBar()->setValue(0);

    // Update Chart Display
    for (int i=0; i<7; result[i] = result[i]*100, ++i) {} // *Display in percentage
    update_chart_display(result, 7);
    this->result = result;
    ui->button_chart_result->setEnabled(true);
}

void MainWindow::on_button_clear_clicked()
{
    // No input
    if (!List->get_status_downloaded()) { show_critical_msg("Error", "No data submitted!"); return; }

    // Delete all data
    delete download;
    download = nullptr;
    List->delete_item();
    mdata->clear();

    // Screen Update
    update_output_screen(0);
    ui->graphicsView->hide(); // *Delete chart record when software close
    ui->label_no_data->setVisible(true);
    ui->button_chart_record->setEnabled(false);
    ui->button_chart_result->setEnabled(false);

    // Status update
    model_generated = false;
    manual_checked = false;

    // UI update
    ui->check_manual_input->setChecked(false);
    ui->price_today->setEnabled(false); ui->price_today->setText("");
    ui->price_day_1->setEnabled(false); ui->price_day_1->setText("");
    ui->price_day_2->setEnabled(false); ui->price_day_2->setText("");
    ui->price_day_3->setEnabled(false); ui->price_day_3->setText("");
    ui->price_day_5->setEnabled(false); ui->price_day_5->setText("");
    ui->price_day_10->setEnabled(false); ui->price_day_10->setText("");
    ui->price_day_20->setEnabled(false); ui->price_day_20->setText("");
    ui->price_day_30->setEnabled(false); ui->price_day_30->setText("");
    ui->button_submit->setEnabled(false);
    ui->button_chart_record->setEnabled(false);

    // Show info message
    ui->statusbar->showMessage("Please download a record first.");
    show_info_msg("Notice", "Submitted records are deleted successfully.");
}

// ====================================================================================
//  UI Displays
// ====================================================================================

void MainWindow::update_output_screen(int condition)
{
    // condition : 0-no_item / 1-display_input / 2-output_display_use

    if (condition == 0)
    {
        // condition=0, display "No data"
        ui->output_screen->setText("\n                                                       ~ No data submitted ~");
    }
    else
    {
        // condition= 1 or 2
        if (condition==1) {ui->output_screen->setText("");} // clear output screen texts

        ui->output_screen->append("Record Submitted");
        ui->output_screen->append("======================================================================");
        ui->output_screen->append("Stock Code : " + List->get_item_code());

        QString display_str = "";
        int item_days = List->get_item_days();
        int count = 1;
        while (true)
        {
            display_str = display_str + "Day " + QString::number(count) + "   -   ";                // Show Day
            display_str = display_str + QString::number(List->get_item_price_from_day(count));      // Show Price
            if ( count == item_days ) { ui->output_screen->append(display_str); break; }            // Print line and break when eof
            else if ( count%3 == 0 ) { ui->output_screen->append(display_str); display_str = ""; }  // Print every 3 days
            else { display_str = display_str + "\t|  "; }                                           // Add spacing
            ++count;
        }

        ui->output_screen->append("");
        if (condition == 1) {ui->output_screen->append("~ End of Record ~");}
        ui->output_screen->verticalScrollBar()->setValue(0);
    }
}

// Chart Display for item stock
void MainWindow::update_chart_display()
{
    // Create New Chart from List records
    QLineSeries* series = new QLineSeries();
    QChart* chart = new QChart();
    for (int i=0; i<List->get_item_days(); ++i)
    {
        series->append(i+1, List->get_item_price_from_day(i+1));
    }

    // Chart Set Up
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Stock Record - " + List->get_item_code());
    ui->graphicsView->setChart(chart);
    ui->graphicsView->setVisible(true);
    ui->label_no_data->setVisible(false);

    // Prevent Memory Leak
    if (update_property) { delete prev_series; delete prev_chart; }
    prev_series = series;
    prev_chart = chart;
    update_property = true;
}

// Chart Display for prediction result
// Need data entry
void MainWindow::update_chart_display(QVector<double> result, int vector_size)
{
    // Create New Chart from Data received
    QLineSeries* series = new QLineSeries();
    QChart* chart = new QChart();
    for (int i=0; i<vector_size; ++i)
    {
        series->append(i+1, result[i]);
    }

    // Chart Set Up
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Future Percentage Change Prediction");
    ui->graphicsView->setChart(chart);
    ui->graphicsView->setVisible(true);

    ui->label_no_data->setVisible(false);

    // Prevent Memory Leak
    if (update_property) { delete prev_series; delete prev_chart; }
    prev_series = series;
    prev_chart = chart;
    update_property = true;
}


void MainWindow::on_button_chart_record_clicked()
{
    update_chart_display();
}

void MainWindow::on_button_chart_result_clicked()
{
    update_chart_display(result, 7);
}
