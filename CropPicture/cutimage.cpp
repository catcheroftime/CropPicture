#include "cutimage.h"
#include "ui_cutimage.h"

#include <QPixmap>

CutImage::CutImage(const QString &filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CutImage)
  , m_orignalImage(filename)
{
    ui->setupUi(this);

    initView();

    int height = m_orignalImage.height();
    int width = m_orignalImage.width();
    this->resize(width + 100, height +150);


    ui->sb_height->setRange(80, height);
    ui->sb_width->setRange(80, width);

    ui->sb_linewidth->setRange(0, 10);
    ui->sb_lineheight->setRange(0, 10);

    ui->lb_showimage->setImage(m_orignalImage);
}

CutImage::~CutImage()
{
    delete ui;
}

QPixmap CutImage::getCropImage()
{
    return ui->lb_showimage->getCroppedImage();
}

void CutImage::initView()
{
    this->setWindowTitle(tr("裁切图片"));
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    ui->rb_square->setChecked(true);
    ui->rb_nofix->setChecked(true);
}

void CutImage::on_cb_difficulty_currentIndexChanged(int index)
{
    disconnect(ui->sb_linewidth,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropInternalLineChange);
    disconnect(ui->sb_lineheight,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropInternalLineChange);

    switch (index) {
    case 0:
        ui->lb_showimage->setCropBoxLine(4,4);
        break;
    case 1:
        ui->lb_showimage->setCropBoxLine(5,5);
        break;
    case 2:
        ui->lb_showimage->setCropBoxLine(6,6);
        break;
    case 3:
        connect(ui->sb_linewidth,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropInternalLineChange);
        connect(ui->sb_lineheight,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropInternalLineChange);
        slotCropInternalLineChange();
        break;
    default:
        break;
    }
}

void CutImage::on_rb_square_clicked()
{
    ui->lb_showimage->setCropBoxShape(CropBox::CropBoxShape::Square);
}

void CutImage::on_rb_rectangle_clicked()
{
    ui->lb_showimage->setCropBoxShape(CropBox::CropBoxShape::Rectangle);
}

void CutImage::on_rb_nofix_clicked()
{
    disconnect(ui->sb_width,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropSizeChange);
    disconnect(ui->sb_height,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropSizeChange);

    ui->lb_showimage->setfixCropBox(this->width(), this->height(), false);
}

void CutImage::on_rb_fixszie_clicked()
{
    connect(ui->sb_width,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropSizeChange);
    connect(ui->sb_height,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CutImage::slotCropSizeChange);

    slotCropSizeChange();
}

void CutImage::slotCropSizeChange()
{
    ui->lb_showimage->setfixCropBox(ui->sb_width->value(), ui->sb_height->value(), true);
}

void CutImage::slotCropInternalLineChange()
{
    ui->lb_showimage->setCropBoxLine(ui->sb_linewidth->value(), ui->sb_lineheight->value());
}

void CutImage::on_ptn_sure_clicked()
{
    this->accept();
}

void CutImage::on_ptn_cancel_clicked()
{
    this->reject();
}
