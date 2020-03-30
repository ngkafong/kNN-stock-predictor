#include "get_data.h"

QUrl get_url(QString name)
{
    QString url = "https://www.alphavantage.co/query?function=TIME_SERIES_DAILY&symbol="+
            name +
            "&outputsize=full&apikey=3XRH6U31Q3GI424R&datatype=csv";
    return url;
}

FileDownloader::FileDownloader(QUrl url, QStatusBar *statusbar, QTextEdit *display_screen) :
    statusbar(statusbar), display_screen(display_screen)
{
    // Get request
    statusbar->showMessage("Downloading...");
    connect(&webman, SIGNAL (finished(QNetworkReply*)),
    this, SLOT (fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(url);
    webman.get(request);
}

FileDownloader::~FileDownloader()
{
    // Do nothing
}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    // Download and Save requested records
    downloadedData = pReply->readAll();
    QFile file("data.csv");
    file.open(QIODevice::WriteOnly);
    file.write(downloadedData);
    file.close();
    pReply->deleteLater();
    emit downloaded();

    statusbar -> showMessage("Download completed.");
    display_screen -> setText("Please wait...");

}

QByteArray FileDownloader::getdownloadedData() const
{
    return downloadedData;
}
