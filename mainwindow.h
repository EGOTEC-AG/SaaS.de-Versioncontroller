#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <filedownloader.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // Settings
    QString appFileName;
    QString filesURL;

    // Sys Vars
    QString versionURL;
    QString onlineVersion;

    QList<QString> pendingDownloads;

    FileDownloader* m_downloadCtrl;

    // Localized Strings
    QString updateInstalled;

    // Methods
    void init();
    void startVersioncontrol();

    bool checkForUpdate();
    QString getLocalVersion();

    void downloadFile(QUrl fileUrl, bool versionCheck);

    void startMainApp();

public slots:
    void saveFile();
    void getOnlineVersion();
    void offlineStart();
};

#endif // MAINWINDOW_H
