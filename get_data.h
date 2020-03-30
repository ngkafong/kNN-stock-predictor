#ifndef CSV_HANDELING_H
#define CSV_HANDELING_H
#include <iostream>
#include <QString>
#include <QVector>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QStatusBar>
#include <QTextEdit>
using namespace std;

QUrl get_url(QString name);

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl imageUrl, QStatusBar* statusbar, QTextEdit *display_screen);
    virtual ~FileDownloader();

    QByteArray getdownloadedData() const;

signals:
    void downloaded();

private slots:
    void fileDownloaded(QNetworkReply* pReply);

private:
    QNetworkAccessManager webman;
    QByteArray downloadedData;
    QStatusBar *statusbar;
    QTextEdit *display_screen;
};

#endif // CSV_HANDELING_H
