#ifndef CUTIMAGE_H
#define CUTIMAGE_H

#include <QDialog>
#include <QPixmap>

namespace Ui {
class CutImage;
}

class CutImage : public QDialog
{
    Q_OBJECT

public:
    explicit CutImage(const QString &filename, QWidget *parent = 0);
    ~CutImage();

private:
    void initView();

private:
    Ui::CutImage *ui;
    QPixmap m_orignalImage;
};

#endif // CUTIMAGE_H
