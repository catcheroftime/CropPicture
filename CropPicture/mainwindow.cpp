#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSettings>

#include "cutimage.h"

#define CONFIGFILE "config.ini"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ptn_openfile_clicked()
{
    QString default_url = getDefaultImportPath();
    QString file = QFileDialog::getOpenFileName(this, tr("打开文件"), default_url);
    if (file.isEmpty())
        return;

    QFileInfo fileinfo{file};
    setDefaultImportPath(fileinfo.path());

    CutImage dialog{file};
    if ( dialog.exec() == QDialog::Accepted) {

    }
}

void MainWindow::on_ptn_exportpath_clicked()
{
    QString filepath = QFileDialog::getExistingDirectory(this);
    if (filepath.isEmpty())
        return;

    ui->le_savepath->setText(filepath);
    setDefaultSavePath(filepath);
}

void MainWindow::initView()
{
    QString savepath = getDefaultSavePath();
    ui->le_savepath->setText(savepath);
}

void MainWindow::setValueForSettings(const QString &target, const QVariant &value)
{
    QString strFileName = QCoreApplication::applicationDirPath() + "/" + CONFIGFILE;
    QSettings settings(strFileName, QSettings::IniFormat);
    settings.setValue(target, value);
}

QString MainWindow::getPathFromSettings(const QString &target)
{
    QString strFileName = QCoreApplication::applicationDirPath() + "/" + CONFIGFILE;
    QSettings settings(strFileName, QSettings::IniFormat);
    QVariant value = settings.value(target);
    QString path = value.toString();
    QDir dir(path);
    if (!dir.exists())
        path = QCoreApplication::applicationDirPath();

    return path;
}

QString MainWindow::getDefaultImportPath()
{
    QString path = getPathFromSettings("Default/ImportPath");
    return path;
}

void MainWindow::setDefaultImportPath(const QString &path)
{
    setValueForSettings("Default/ImportPath", path);
}

QString MainWindow::getDefaultSavePath()
{
    QString path = getPathFromSettings("Default/SavePath");
    return path;
}

void MainWindow::setDefaultSavePath(const QString &path)
{
    setValueForSettings("Default/SavePath", path);
}
