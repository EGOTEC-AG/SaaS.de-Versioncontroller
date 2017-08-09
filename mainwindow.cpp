#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filedownloader.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QStringList>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include <QProcess>

#include <QLocale>

#ifdef _WIN32
    #include <Windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

void MainWindow::init() {
    // Settings
#ifdef _WIN32
    MainWindow::appFileName = "Timerecording.exe";
    MainWindow::filesURL = "http://files.saas.de/saasde/tray/windows/";
#elif __APPLE__
    MainWindow::appFileName = "Timerecording.app";
    MainWindow::filesURL = "http://files.saas.de/saasde/tray/mac/";
#endif

    // Check if app exists
    if (QFile(qApp->applicationDirPath() + "/" + appFileName).exists()) {
        versionURL = filesURL + "version";

        downloadFile(versionURL, true);
    } else {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "SaaS.de Versioncontrol", "Error!\n\nContact the support!", QMessageBox::Ok | QMessageBox::Ok);
        if (resBtn == QMessageBox::Ok) {
            qApp->closeAllWindows();
            qApp->quit();
        }
    }
}

void MainWindow::startVersioncontrol() {
    if (checkForUpdate()) {
        pendingDownloads.append("version");
        pendingDownloads.append(MainWindow::appFileName);
#ifdef _WIN32
        pendingDownloads.append("Uninstaller.exe");
#endif

        QUrl url(filesURL);
        QNetworkRequest req(url);
        QNetworkAccessManager http;
        QNetworkReply *netReply = http.get(req);

        QEventLoop loop;
        QObject::connect(netReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        QByteArray responseData = netReply->readAll();
        netReply->close();
        delete netReply;

        std::string result = QString(responseData).toStdString();
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(result));

        if (doc.isArray()) {
            QJsonArray files = doc.array();
            for (unsigned int i = 0; i < files.size(); i++) {
                QString file = ((QJsonValue) files[i]).toString();
                pendingDownloads.push_back(file);
            }
        }

        downloadFile(filesURL + pendingDownloads[0], false);
    } else {
        startMainApp();
    }
}

bool MainWindow::checkForUpdate() {
    if (QFile(qApp->applicationDirPath() + "/version").exists()) {
        if (getLocalVersion() == onlineVersion) {
            return false;
        }
    }
    return true;
}

void MainWindow::downloadFile(QUrl fileUrl, bool versionCheck) {
    m_downloadCtrl = new FileDownloader(fileUrl, this);

    connect(m_downloadCtrl, SIGNAL(connectionError()), this, SLOT(offlineStart()));

    if (versionCheck) {
        connect(m_downloadCtrl, SIGNAL(downloaded()), this, SLOT(getOnlineVersion()));
    } else {
        connect(m_downloadCtrl, SIGNAL(downloaded()), this, SLOT(saveFile()));
    }
}

void MainWindow::saveFile() {
     QString jsonPath = pendingDownloads[0];

     QString folder = qApp->applicationDirPath() + "/";
    if (jsonPath.contains("/")) {
        QStringList tmp = jsonPath.split("/");

        folder += tmp[0] + "/";
        jsonPath = tmp[1];

        if (!QDir(folder).exists()) {
            QDir().mkdir(folder);
        }

        if(tmp.count() == 3){
            folder += tmp[1] + "/";
            jsonPath = tmp[2];

            if (!QDir(folder).exists()) {
                QDir().mkdir(folder);
            }
        }
    }

    if (QFile(folder +jsonPath).exists()) {
        QFile::remove(folder + jsonPath);
    }

    QFile* file = new QFile(folder + jsonPath);
    file->open(QIODevice::WriteOnly);
    file->write(m_downloadCtrl->downloadedData());
    file->close();

    MainWindow::pendingDownloads.removeFirst();
    if (pendingDownloads.count() != 0) {
        downloadFile(filesURL + pendingDownloads[0], false);
    } else {
        startMainApp();
    }
}

void MainWindow::getOnlineVersion() {
    onlineVersion = m_downloadCtrl->downloadedData().replace("\n", "");

    startVersioncontrol();
}

void MainWindow::offlineStart() {
    startMainApp();
}

QString MainWindow::getLocalVersion() {
    QFile localVersion(qApp->applicationDirPath() + "/version");
    if(!localVersion.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "Error", localVersion.errorString());
    }
    QTextStream in(&localVersion);

    return in.readLine().replace(QString("\n"), QString(""));
}

void MainWindow::startMainApp() {
    QProcess* process = new QProcess();
    process->startDetached(appFileName);

    qApp->closeAllWindows();
    qApp->quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}
