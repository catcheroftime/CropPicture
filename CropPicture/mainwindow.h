#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_ptn_openfile_clicked();

    void on_ptn_exportpath_clicked();

private:
    void initView();
    void initStyle();
    void setValueForSettings(const QString &target, const QVariant &value);
    QString getPathFromSettings(const QString &target);

    QString getDefaultImportPath();
    void setDefaultImportPath(const QString &path);

    QString getDefaultSavePath();
    void setDefaultSavePath(const QString &path);



private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
