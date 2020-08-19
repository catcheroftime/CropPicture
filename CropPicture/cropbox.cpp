#include "cropbox.h"

#include <QDebug>
#include <QPainterPath>
#include <QPainter>
#include <QMouseEvent>

#define LINEWIDTH 1
#define SPACING   2
#define POINTSIZE 5

CropBox::CropBox(QWidget *parent)
    : QWidget(parent)
    , m_shape(CropBoxShape::Rect)
    , m_mode(ZoomMode::Free)
    , m_bFixSized(false)
    , m_bDrawInternalLines(true)
    , m_widthCount(4)
    , m_heightCount(4)
    , m_bMovingFlag(false)
    , m_curDirec(NONE)
{
    this->installEventFilter(this);
    this->setMouseTracking(true);
}

CropBox::~CropBox()
{

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

void CropBox::setZoomMode(CropBox::ZoomMode mode)
{
    m_mode = mode;
}

void CropBox::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
}

void CropBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    drawBackground(painter);
    if (m_bDrawInternalLines)
        drawInternalLines(painter);

    if (m_shape == Round) {
        drawRound(painter);
    }


    drawBorder(painter);
    drawPoints(painter);

    drawSizeText(painter);
}

void CropBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bMovingFlag = true;
        m_dragPosition = event->globalPos() - this->pos();
    }
    event->accept();
}


void CropBox::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    QPoint global_point = event->globalPos();

    if (!m_bMovingFlag)
        setDirection(point);
    else {
        if (m_mode == Free)
            resizeRectangle(global_point, point);
        else
            resizeSquare(global_point, point);
    }

    event->accept();
}


void CropBox::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    if(event->button()==Qt::LeftButton) {
        m_bMovingFlag = false;
    }

    event->accept();
}

void CropBox::drawBackground(QPainter &painter)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(2, 2, this->width()-2, this->height()-2);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QColor(0, 0, 0, 0));
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

void CropBox::drawBorder(QPainter &painter)
{
    painter.setPen( QPen{QColor{3,125,203},SPACING});
    painter.drawRect( SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2 );
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

void CropBox::drawRound(QPainter &painter)
{
    painter.setPen( QPen{QColor{255,255,255},LINEWIDTH,Qt::DashLine});
    painter.drawEllipse(SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2 );


    QPainterPath border, round;
    border.setFillRule(Qt::WindingFill);
    border.addRect(0, 0, this->width(), this->height());

    round.setFillRule(Qt::WindingFill);
    round.addEllipse(SPACING, SPACING, this->width()-SPACING*2,  this->height()-SPACING*2);

    QPainterPath end_path = border.subtracted(round);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(end_path, QColor(0, 0, 0, 100));
}

void CropBox::drawSizeText(QPainter &painter)
{
    painter.setPen( QPen{QColor{255,0,0}});
    QString showText = QString("(") + QString::number(this->width()) + "," + QString::number(this->height()) + ")";
    QPointF topleft{(qreal)this->width()-(qreal)MINSIZE, (qreal)this->height()-(qreal)20};
    QSizeF size{MINSIZE,20};
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
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
    }
    else if(width - PADDING <= point.x() && width >= point.x() && heigth - PADDING <= point.y() && heigth >= point.y())
    {
        m_curDirec = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
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

void CropBox::resizeRectangle(QPoint global_point, QPoint local_point)
{
    QRect rectMove = this->geometry();

    QPoint parent_point = mapToParent(local_point);

    if (m_bMovingFlag)
    {
        QWidget* parent_widget = (QWidget *)this->parent();
        switch(m_curDirec)
        {
        case NONE: {
            QPoint end_point = global_point - m_dragPosition ;
            if (parent_widget) {
                int new_x = judgePosition(end_point.x(), 0, parent_widget->width()-this->width());
                end_point.setX(new_x);

                int new_y = judgePosition(end_point.y(), 0, parent_widget->height()-this->height());
                end_point.setY( new_y );
            }
            move( end_point );
            break;
        }
        case LEFTTOP:
            if (parent_widget) {
                if (parent_point.x() < 0 || parent_point.y() < 0)
                    return;
            }
            if ( rectMove.right() - parent_point.x()  < MINSIZE || rectMove.bottom() - parent_point.y() < MINSIZE)
                return;

            rectMove.setLeft(parent_point.x() );
            rectMove.setTop(parent_point.y());
            break;
        case RIGHTBOTTOM: {
            if (parent_widget) {
                if ( parent_point.x() > parent_widget->width() || parent_point.y() > parent_widget->height())
                    return;
            }
            if ( parent_point.x()  - rectMove.left() < MINSIZE || parent_point.y() - rectMove.top() < MINSIZE)
                return;

            rectMove.setRight(parent_point.x() );
            rectMove.setBottom(parent_point.y());
            break;
        }
        case LEFTBOTTOM:
            if (parent_widget) {
                if (parent_point.x() < 0 || parent_point.y() > parent_widget->height())
                    return;
            }
            if ( rectMove.right() - parent_point.x() < MINSIZE || parent_point.y() - rectMove.top() < MINSIZE)
                return;

            rectMove.setLeft(parent_point.x() );
            rectMove.setBottom(parent_point.y());
            break;
        case RIGHTTOP:
            if (parent_widget) {
                if (parent_point.y() < 0 || parent_point.x() > parent_widget->width())
                    return;
            }
            if ( parent_point.x()- rectMove.left() < MINSIZE || rectMove.bottom() - parent_point.y() < MINSIZE)
                return;
            rectMove.setRight(parent_point.x() );
            rectMove.setTop(parent_point.y());
            break;
        case LEFT: {
            if (parent_widget) {
                if ( parent_point.x() < MINSIZE )
                    return;
            }

            if ( rectMove.right() - parent_point.x() < MINSIZE )
                return;

            rectMove.setLeft(parent_point.x()  );
            break;
        }
        case UP:
            if (parent_widget) {
                if (parent_point.y() < 0)
                    return;
            }
            if ( rectMove.bottom() - parent_point.y() < MINSIZE)
                return;

            rectMove.setTop(parent_point.y());
            break;
        case RIGHT: {
            if (parent_widget) {
                if ( parent_point.x() > parent_widget->width())
                    return;
            }
            if ( parent_point.x() - rectMove.left() < MINSIZE)
                return;

            rectMove.setRight(parent_point.x() );
            break;
        }
        case DOWN: {
            if (parent_widget) {
                if ( parent_point.y() > parent_widget->height())
                    return;
            }
            if ( parent_point.y() - rectMove.top() < MINSIZE)
                return;

            rectMove.setBottom(parent_point.y());
            break;
        }
        default:
            break;

        }
    }
    if (m_curDirec != NONE)
        this->setGeometry(rectMove);
}

void CropBox::resizeSquare(QPoint global_point, QPoint local_point)
{
    QRect rectMove = this->geometry();

    QPoint parent_point = mapToParent(local_point);

    if (m_bMovingFlag)
    {
        QWidget* parent_widget = (QWidget *)this->parent();
        switch(m_curDirec)
        {
        case NONE: {
            QPoint end_point = global_point - m_dragPosition ;
            if (parent_widget) {
                int new_x = judgePosition(end_point.x(), 0, parent_widget->width()-this->width());
                end_point.setX(new_x);

                int new_y = judgePosition(end_point.y(), 0, parent_widget->height()-this->height());
                end_point.setY( new_y );
            }
            move( end_point );
            break;
        }
        case LEFTTOP: {
            if (parent_widget) {
                if (parent_point.x() < 0 || parent_point.y() < 0 || parent_point.x()- rectMove.right() + rectMove.bottom() < 0)
                    return;
            }
            if ( rectMove.right() - parent_point.x()  < MINSIZE || rectMove.bottom() - parent_point.y() < MINSIZE)
                return;

            rectMove.setLeft(parent_point.x() );
            rectMove.setTop(parent_point.x()- rectMove.right() + rectMove.bottom());
            break;
        }

        case RIGHTBOTTOM: {
            if (parent_widget) {
                if ( parent_point.x() > parent_widget->width() || parent_point.y() > parent_widget->height() || parent_point.x()- rectMove.left() + rectMove.top()> parent_widget->height())
                    return;
            }
            if ( parent_point.x()  - rectMove.left() < MINSIZE || parent_point.y() - rectMove.top() < MINSIZE)
                return;

            rectMove.setRight( parent_point.x());
            rectMove.setBottom(parent_point.x()- rectMove.left() + rectMove.top());
            break;
        }
        case LEFTBOTTOM:
            if (parent_widget) {
                if (parent_point.x() < 0 || parent_point.y() > parent_widget->height() ||rectMove.top() - parent_point.x() + rectMove.right() > parent_widget->height() )
                    return;
            }
            if ( rectMove.right() - parent_point.x() < MINSIZE || parent_point.y() - rectMove.top() < MINSIZE)
                return;

            rectMove.setLeft(parent_point.x() );
            rectMove.setBottom(rectMove.top() - parent_point.x() + rectMove.right() );
            break;
        case RIGHTTOP:
            if (parent_widget) {
                if (parent_point.y() < 0 || parent_point.x() > parent_widget->width() || rectMove.bottom() - parent_point.x() + rectMove.left() < 0)
                    return;
            }
            if ( parent_point.x()- rectMove.left() < MINSIZE || rectMove.bottom() - parent_point.y() < MINSIZE)
                return;

            rectMove.setRight(parent_point.x() );
            rectMove.setTop(rectMove.bottom() - parent_point.x() + rectMove.left());
            break;
        case LEFT: {
            if (parent_widget) {
                if ( parent_point.x() < 0 || parent_point.x()- rectMove.right() + rectMove.bottom() < 0)
                    return;
            }

            if ( rectMove.right() - parent_point.x() < MINSIZE )
                return;

            rectMove.setLeft(parent_point.x() );
            rectMove.setTop(parent_point.x()- rectMove.right() + rectMove.bottom());
            break;
        }
        case UP:
            if (parent_widget) {
                if (parent_point.y() < 0 || rectMove.left() - parent_point.y() + rectMove.bottom() > parent_widget->width() )
                    return;
            }
            if ( rectMove.bottom() - parent_point.y() < MINSIZE)
                return;

            rectMove.setTop(parent_point.y() );
            rectMove.setRight( rectMove.left() - parent_point.y() + rectMove.bottom());
            break;
        case RIGHT: {
            if (parent_widget) {
                if ( parent_point.x() > parent_widget->width() || parent_point.x()- rectMove.left() + rectMove.top() > parent_widget->height())
                    return;
            }
            if ( parent_point.x() - rectMove.left() < MINSIZE)
                return;

            rectMove.setRight( parent_point.x());
            rectMove.setBottom(parent_point.x()- rectMove.left() + rectMove.top());
            break;
        }
        case DOWN: {
            if (parent_widget) {
                if ( parent_point.y() > parent_widget->height() || rectMove.top() - parent_point.y() + rectMove.right()< 0 )
                    return;
            }
            if ( parent_point.y() - rectMove.top() < MINSIZE)
                return;

            rectMove.setBottom( parent_point.y());
            rectMove.setLeft(rectMove.top() - parent_point.y() + rectMove.right() );
            break;
        }
        default:
            break;
        }
    }
    if (m_curDirec != NONE)
        this->setGeometry(rectMove);
}

int CropBox::judgePosition(int origin, int min, int max)
{
    if (origin < min)
        return min;

    if (origin > max)
        return max;

    return origin;
}
