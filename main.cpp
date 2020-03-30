#include <QApplication>
#include <QPushButton>

#include "mainwindow.h"

int main(int argc, char **argv)
{

    QApplication app (argc, argv);

    MainWindow window;
    window.show();

    return app.exec(); // This calls the event loop to launch

}
