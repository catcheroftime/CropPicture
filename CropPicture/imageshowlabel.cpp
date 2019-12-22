#include "imageshowlabel.h"

#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>

#include "cropbox.h"

ImageShowLabel::ImageShowLabel(QWidget *parent) :
    QLabel(parent)
  , m_pCropBox(new CropBox(this))
  , m_widthCount(3)
  , m_heightCount(3)
{
    m_pCropBox->setMouseTracking(true);
    m_pCropBox->resize(100, 100);
    m_pCropBox->move(0, 0);
    m_pCropBox->show();
}

void ImageShowLabel::setImage(const QPixmap &image)
{
    this->setPixmap(image);
}

//QPixmap ImageShowLabel::getCroppedImage()
//{

//}

void ImageShowLabel::setCropBoxLine(const int& widthcount, const int& heightcount)
{
    m_pCropBox->setCropBoxLine(widthcount, heightcount );

}

void ImageShowLabel::setCropBoxShape(CropBox::CropBoxShape shape)
{
    m_pCropBox->setCropBoxShape(shape);
}

void ImageShowLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainterPath border, cropbox;
    border.setFillRule(Qt::WindingFill);
    border.addRect(0, 0, this->width(), this->height());

    cropbox.setFillRule(Qt::WindingFill);
    cropbox.addRect(m_pCropBox->pos().x()+1,m_pCropBox->pos().y()+1, m_pCropBox->width()-2, m_pCropBox->height()-2);

    QPainterPath end_path = border.subtracted(cropbox);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(end_path, QColor(0, 0, 0, 100));
}

void ImageShowLabel::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}
