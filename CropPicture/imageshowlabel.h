#ifndef IMAGESHOWLABEL_H
#define IMAGESHOWLABEL_H

#include <QLabel>
#include "cropbox.h"

class QPixmap;

class ImageShowLabel : public QLabel
{
public:
    ImageShowLabel(QWidget *parent = 0);

    void setImage(const QPixmap &image);
    QPixmap getCroppedImage();

    void setCropBoxLine(const int & widthcount,const int& heightcount);
    void setCropBoxShape(CropBox::CropBoxShape shape  = CropBox::Rect);

    void setfixCropBox(const int & width, const int& height, bool fixed = true);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void initView();

private:
    CropBox * m_pCropBox;
    QPixmap m_orginalImg;
};

#endif // IMAGESHOWLABEL_H
