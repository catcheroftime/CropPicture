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

public:
    QPixmap getCropImage();

private slots:
    void on_cb_difficulty_currentIndexChanged(int index);

    void on_rb_square_clicked();
    void on_rb_rectangle_clicked();

    void on_rb_nofix_clicked();
    void on_rb_fixszie_clicked();

    void slotCropSizeChange();
    void slotCropInternalLineChange();
    void on_ptn_sure_clicked();

    void on_ptn_cancel_clicked();

    void on_rb_ratio_clicked();

    void on_rb_free_clicked();

    void on_rb_fixed_clicked();

private:
    void initView();
    void initStyle();

private:
    Ui::CutImage *ui;
    QPixmap m_orignalImage;
};

#endif // CUTIMAGE_H
