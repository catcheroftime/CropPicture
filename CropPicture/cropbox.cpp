#include "cropbox.h"

#include <QDebug>
#include <QPainterPath>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>

#define LINEWIDTH 1
#define SPACING   2
#define POINTSIZE 5
#define PADDING   4

CropBox::CropBox(QWidget *parent)
    : QWidget(parent)
    , m_shape(CropBoxShape::Rect)
    , m_zoomMode(ZoomMode::Free)
    , m_keyPressZoomMode(ZoomMode::Free)
    , m_minWidth(80)
    , m_minHeight(80)
    , m_heightwidthRatio(1.0)
    , m_ratioMinWidth(80.0)
    , m_ratioMinHeight(80.0)
    , m_bFixSized(false)
    , m_bDrawInternalLines(true)
    , m_widthCount(4)
    , m_heightCount(4)
    , m_bMovingFlag(false)
    , m_curDirec(NONE)
{
    this->setMouseTracking(true);
    this->setEnableKeyPressEvent(true);
}

CropBox::~CropBox()
{

}

void CropBox::setMinSize(const int &width, const int &height)
{
    QWidget* parent_widget = (QWidget *)this->parent();
    if (parent_widget) {
        m_minWidth = judgePosition(width, parent_widget->width()<10?parent_widget->width()/2:10, parent_widget->width());
        m_minHeight =  judgePosition(height, parent_widget->height()<10?parent_widget->height()/2:10, parent_widget->height());
    }

}

void CropBox::fixCropBox(bool fixsized)
{
    m_bFixSized = fixsized;
}

void CropBox::drawCropBoxInternalLines(bool drawable)
{
    m_bDrawInternalLines = drawable;
}

void CropBox::setCropBoxLine(const int &widthcount, const int &heightcount)
{
    m_widthCount = widthcount;
    m_heightCount = heightcount;
}

void CropBox::setCropBoxShape(CropBox::CropBoxShape shape)
{
    m_shape = shape;
}

CropBox::CropBoxShape CropBox::getCropBoxShape()
{
    return m_shape;
}

void CropBox::setZoomMode(CropBox::ZoomMode mode)
{
    m_zoomMode = mode;

    if (mode == Square)
        m_heightwidthRatio = 1.0;
    else
        m_heightwidthRatio = (double)this->height()/this->width();

    m_ratioMinWidth = m_minWidth * m_heightwidthRatio > m_minHeight? m_minWidth : m_minHeight / m_heightwidthRatio;
    m_ratioMinHeight = m_minWidth * m_heightwidthRatio > m_minHeight? m_minWidth * m_heightwidthRatio : m_minHeight;
}

void CropBox::setEnableKeyPressEvent(bool enabled)
{
    if (enabled)
        this->grabKeyboard();
    else
        this->releaseKeyboard();
}


void CropBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (m_bDrawInternalLines)
        drawInternalLines(painter);

    drawBorder(painter);
    drawPoints(painter);
    drawSizeText(painter);
}

void CropBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bMovingFlag = true;
        m_dragPosition = event->globalPos() - this->pos();
    }
    event->accept();
}


void CropBox::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    QPoint parent_point = mapToParent(point);
    QPoint global_point = event->globalPos();

    if (!m_bMovingFlag) {
        setDirection(point);
    } else {
        if (m_curDirec == NONE)
            handleMove(global_point);
        else
            handleResize(parent_point);
    }

    event->accept();
}


void CropBox::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    if(event->button()==Qt::LeftButton)
        m_bMovingFlag = false;

    event->accept();
}

void CropBox::keyPressEvent(QKeyEvent *event)
{
    m_keyPressZoomMode = m_zoomMode;

    if (event->key() == Qt::Key_Control) {
        this->setZoomMode(FixedRatio);
        return;
    } else if(event->key() == Qt::Key_Alt) {
        this->setZoomMode(Square);
        return;
    } else {
        QWidget::keyPressEvent(event);
    }
}

void CropBox::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Alt) {
        this->setZoomMode(m_keyPressZoomMode);
        return;
    }

    QWidget::keyPressEvent(event);
}

void CropBox::drawBorder(QPainter &painter)
{
    painter.setPen( QPen{QColor{3,125,203},SPACING});
    painter.drawRect( SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2 );

    if (m_shape == Round) {
        painter.setPen( QPen{QColor{255,255,255},LINEWIDTH,Qt::DashLine});
        painter.drawEllipse(SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2 );
    }
}

void CropBox::drawPoints(QPainter &painter)
{
    painter.setPen( QPen{QColor{3,125,203},POINTSIZE});
    painter.drawPoint(SPACING,SPACING);
    painter.drawPoint(this->width()/2, SPACING);
    painter.drawPoint(this->width()-SPACING, SPACING);
    painter.drawPoint(SPACING, this->height()/2);
    painter.drawPoint(SPACING, this->height()-SPACING);
    painter.drawPoint(this->width()-SPACING, this->height()/2);
    painter.drawPoint(this->width()-SPACING, this->height()-SPACING);
    painter.drawPoint(this->width()/2, this->height()-SPACING);
}

void CropBox::drawInternalLines(QPainter &painter)
{
    QPainterPath cropbox_path;
    if (m_shape == Round)
        cropbox_path.addEllipse(SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2);
    else
        cropbox_path.addRect(SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2);

    painter.setClipPath(cropbox_path);
    painter.setClipping(true);

    painter.setPen( QPen{QColor{230,230,230},LINEWIDTH,Qt::DashLine});
    for (int i=1; i<m_widthCount; i++) {
        int width = this->width() / m_widthCount;
        painter.drawLine( i*width,  SPACING , i*width , this->height()-SPACING);
    }

    for (int i=1; i<m_heightCount; i++) {
        int heigth = this->height()/ m_heightCount;
        painter.drawLine( SPACING ,i*heigth,   this->width()- SPACING, i*heigth);
    }

    painter.setClipping(false);
}

void CropBox::drawSizeText(QPainter &painter)
{
    painter.setPen( QPen{QColor{255,0,0}});
    QString showText = QString("(") + QString::number(this->width()) + "," + QString::number(this->height()) + ")";
    QPointF topleft{(qreal)this->width()-(qreal)m_minWidth, (qreal)this->height()-(qreal)20};
    QSizeF size{(qreal)m_minWidth,20};
    QRectF position {topleft, size};
    QTextOption option{Qt::AlignVCenter | Qt::AlignRight };
    painter.drawText(position, showText, option);
}

void CropBox::setDirection(QPoint point)
{
    if (m_bFixSized) {
        m_curDirec = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
        return;
    }

    int width = this->width();
    int heigth = this->height();

    if ( PADDING >= point.x() && 0 <= point.x() &&  PADDING  >= point.y() && 0 <= point.y())
    {
        m_curDirec = LEFTTOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(width - PADDING <= point.x() && width >= point.x() && heigth - PADDING <= point.y() && heigth >= point.y())
    {
        m_curDirec = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if(PADDING >= point.x() && 0 <= point.x() && heigth - PADDING <= point.y() && heigth >= point.y())
    {
        m_curDirec = LEFTBOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(PADDING >= point.y() && 0 <= point.y() && width - PADDING <= point.x() && width >= point.x())
    {
        m_curDirec = RIGHTTOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(PADDING >= point.x() && 0 <= point.x())
    {
        m_curDirec = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(PADDING >= point.y() && 0 <= point.y())
    {
        m_curDirec = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if(width - PADDING <= point.x() && width >= point.x())
    {
        m_curDirec = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(heigth - PADDING <= point.y() && heigth >= point.y())
    {
        m_curDirec = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else
    {
        m_curDirec = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void CropBox::handleMove(QPoint mouse_globalpos)
{
    QWidget* parent_widget = (QWidget *)this->parent();
    QPoint end_point = mouse_globalpos - m_dragPosition ;
    if (parent_widget) {
        int new_x = judgePosition(end_point.x(), 0, parent_widget->width()-this->width());
        end_point.setX(new_x);

        int new_y = judgePosition(end_point.y(), 0, parent_widget->height()-this->height());
        end_point.setY( new_y );
    }
    move( end_point );
}

void CropBox::handleResize(QPoint mouse_parentpos)
{
    if (!m_bMovingFlag)
        return;

    QRect rectMove = this->geometry();

    QPoint valid_point{mouse_parentpos} ;

    QWidget* parent_widget = (QWidget *)this->parent();
    valid_point.setX( judgePosition(valid_point.x(), 0, parent_widget->width()) );
    valid_point.setY( judgePosition(valid_point.y(), 0, parent_widget->height()) );

    switch(m_curDirec) {
    case UP:
        handleResizeUp(valid_point, rectMove, parent_widget);
        break;
    case DOWN:
        handleResizeDown(valid_point, rectMove, parent_widget);
        break;
    case LEFT:
        handleResizeLeft(valid_point, rectMove, parent_widget);
        break;
    case RIGHT:
        handleResizeRight(valid_point, rectMove, parent_widget);
        break;
    case RIGHTTOP:
        handleResizeRightTop(valid_point, rectMove, parent_widget);
        break;
    case RIGHTBOTTOM:
        handleResizeRightBottom(valid_point, rectMove, parent_widget);
        break;
    case LEFTTOP:
        handleResizeLeftTop(valid_point, rectMove, parent_widget);
        break;
    case LEFTBOTTOM:
        handleResizeLeftBottom(valid_point, rectMove, parent_widget);
        break;
    default:
        break;
    }

    this->setGeometry(rectMove);
}

void CropBox::handleResizeUp(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    if (m_zoomMode !=  Free) {
        handleResizeRightTop(valid_point, rectNew, parent_widget);
        return;
    }

    if (rectNew.bottom() - valid_point.y() + 1  <= m_minHeight)
        valid_point.setY( rectNew.bottom() - m_minHeight + 1);

    rectNew.setTop( valid_point.y() );
}

void CropBox::handleResizeDown(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    if (m_zoomMode !=  Free) {
        handleResizeLeftBottom(valid_point, rectNew, parent_widget);
        return;
    }

    if ( valid_point.y() - rectNew.top() + 1 <= m_minHeight)
        valid_point.setY( rectNew.top() + m_minHeight - 1);

    rectNew.setBottom( valid_point.y() );
}

void CropBox::handleResizeLeft(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    if (m_zoomMode !=  Free) {
        handleResizeLeftTop(valid_point, rectNew, parent_widget);
        return;
    }

    if ( rectNew.right() - valid_point.x() + 1 <= m_minWidth)
        valid_point.setX( rectNew.right() - m_minWidth + 1);

    rectNew.setLeft( valid_point.x() );
}

void CropBox::handleResizeRight(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    if (m_zoomMode !=  Free) {
        handleResizeRightBottom(valid_point, rectNew, parent_widget);
        return;
    }

    if ( valid_point.x() - rectNew.left() + 1 <= m_minWidth)
        valid_point.setX( rectNew.left() + m_minWidth - 1);

    rectNew.setRight( valid_point.x() );
}

void CropBox::handleResizeRightTop(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    if (m_zoomMode !=  Free) {
        if (valid_point.x() - rectNew.left() + 1 <= m_ratioMinWidth)
            valid_point.setX( rectNew.left() + m_ratioMinWidth - 1);

        if (rectNew.bottom() - valid_point.y() + 1 <= m_ratioMinHeight)
            valid_point.setY( rectNew.bottom() - m_ratioMinHeight + 1);

        int right = (rectNew.bottom()- valid_point.y() + 1)/m_heightwidthRatio + rectNew.left() - 1 ;
        if ( right > parent_widget->width() ) {
            right = parent_widget->width();
            valid_point.setY( rectNew.bottom() - (parent_widget->width() - rectNew.left() + 1)*m_heightwidthRatio + 1 );
        }

        valid_point.setX( right );
    } else {
        if (valid_point.x() - rectNew.left() + 1 <= m_minWidth)
            valid_point.setX( rectNew.left() + m_minWidth - 1);

        if (rectNew.bottom() - valid_point.y() + 1 <= m_minHeight )
            valid_point.setY( rectNew.bottom() - m_minHeight + 1 );

    }

    rectNew.setRight(valid_point.x());
    rectNew.setTop(valid_point.y());
}

void CropBox::handleResizeRightBottom(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    if (m_zoomMode !=  Free) {
        if (valid_point.x() - rectNew.left() + 1 <= m_ratioMinWidth)
            valid_point.setX( m_ratioMinWidth + rectNew.left() - 1);

        if (valid_point.y() - rectNew.top() + 1 <= m_ratioMinHeight)
            valid_point.setY( rectNew.top() + m_ratioMinHeight - 1 );

        int bottom =  (valid_point.x() - rectNew.left() + 1)*m_heightwidthRatio  + rectNew.top() - 1;
        if ( bottom >  parent_widget->height()) {
            bottom = parent_widget->height();
            valid_point.setX( rectNew.left() + (parent_widget->height() - rectNew.top() + 1) / m_heightwidthRatio - 1);
        }
        valid_point.setY( bottom );
    } else {
        if (valid_point.x() - rectNew.left() + 1 <= m_minWidth)
            valid_point.setX( m_minWidth + rectNew.left() - 1 );

        if (valid_point.y() - rectNew.top() + 1 <= m_minHeight)
            valid_point.setY( rectNew.top() + m_minHeight - 1);
    }

    rectNew.setRight(valid_point.x());
    rectNew.setBottom(valid_point.y());
}

void CropBox::handleResizeLeftTop(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    Q_UNUSED(parent_widget);
    if (m_zoomMode !=  Free) {
        if (rectNew.right() - valid_point.x() + 1 <= m_ratioMinWidth)
            valid_point.setX( rectNew.right() - m_ratioMinWidth + 1);

        if (rectNew.bottom() - valid_point.y()+1 <= m_ratioMinHeight)
            valid_point.setY( rectNew.top() - m_ratioMinHeight + 1);

        int top = rectNew.bottom()  - (rectNew.right() - valid_point.x() + 1) * m_heightwidthRatio + 1;
        if ( top < 0 ) {
            top = 0;
            valid_point.setX( rectNew.right() + 1 - (rectNew.bottom() + 1)/m_heightwidthRatio);
        }
        valid_point.setY( top );
    } else {
        if (rectNew.right() - valid_point.x() + 1 <= m_minWidth)
            valid_point.setX( rectNew.right() - m_minWidth + 1 );

        if (rectNew.bottom() - valid_point.y() + 1 <= m_minHeight)
            valid_point.setY( rectNew.bottom() - m_minHeight + 1);
    }

    rectNew.setLeft(valid_point.x());
    rectNew.setTop(valid_point.y());
}

void CropBox::handleResizeLeftBottom(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget)
{
    Q_UNUSED(parent_widget);

    if (m_zoomMode !=  Free) {
        if (rectNew.right() - valid_point.x() + 1 <= m_ratioMinWidth)
            valid_point.setX( rectNew.right() - m_ratioMinWidth + 1 );

        if (valid_point.y() - rectNew.top() + 1 <= m_ratioMinHeight)
            valid_point.setY( rectNew.top() + m_ratioMinHeight - 1);

        int left = rectNew.right() - (valid_point.y() - rectNew.top() + 1)/m_heightwidthRatio + 1;
        if ( left < 0 ) {
            left = 0;
            valid_point.setY( (rectNew.right()+1) * m_heightwidthRatio + rectNew.top() - 1 );
        }
        valid_point.setX( left ) ;
    } else {
        if (rectNew.right() - valid_point.x() + 1 <= m_minWidth)
            valid_point.setX( rectNew.right() - m_minWidth + 1);

        if (valid_point.y() - rectNew.top() + 1 <= m_minHeight)
            valid_point.setY( rectNew.top() + m_minHeight - 1);
    }

    rectNew.setLeft(valid_point.x());
    rectNew.setBottom(valid_point.y());
}

inline int CropBox::judgePosition(int origin, int min, int max)
{
    if (origin < min)  return min;
    if (origin > max)  return max;
    return origin;
}
