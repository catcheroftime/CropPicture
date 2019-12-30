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
    m_orginalImg = image;
    this->setPixmap(image);
}

QPixmap ImageShowLabel::getCroppedImage()
{
    return m_orginalImg.copy(m_pCropBox->pos().x(), m_pCropBox->pos().y(), m_pCropBox->width(), m_pCropBox->height() );
}

void ImageShowLabel::setCropBoxLine(const int& widthcount, const int& heightcount)
{
    m_pCropBox->setCropBoxLine(widthcount, heightcount );
    update();
}

void ImageShowLabel::setCropBoxShape(CropBox::CropBoxShape shape)
{
    m_pCropBox->setCropBoxShape(shape);
    update();
}

void ImageShowLabel::setfixCropBox(const int &width, const int &height, bool fixed)
{
    if (fixed)
        m_pCropBox->resize(width,height);
    else
        m_pCropBox->resize(100, 100);

    if (m_pCropBox->width() + m_pCropBox->pos().x() > this->width() || m_pCropBox->height() + m_pCropBox->pos().y() > this->height())
        m_pCropBox->move(0, 0);

    m_pCropBox->fixCropBox(fixed);
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
