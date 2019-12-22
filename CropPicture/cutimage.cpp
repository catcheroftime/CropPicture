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

    ui->lb_showimage->setImage(m_orignalImage);
}

CutImage::~CutImage()
{
    delete ui;
}

void CutImage::initView()
{
    this->setWindowTitle(tr("裁切图片"));
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);

    ui->rb_square->setChecked(true);
    ui->rb_nofix->setChecked(true);
}
