#ifndef CROPBOX_H
#define CROPBOX_H

#include <QWidget>
#include <QPainterPath>

class CropBox : public QWidget
{
    Q_OBJECT

    enum Direction { UP=0, DOWN, LEFT, RIGHT, LEFTTOP, LEFTBOTTOM, RIGHTBOTTOM, RIGHTTOP, NONE };

public:
    CropBox(QWidget *parent);
    ~CropBox();

    enum CropBoxShape {
        Rect,
        Round
    };

    enum ZoomMode {
        Free,
        FixedRatio,
        Square,
    };

    void setMinSize(const int &width, const int &height);
    void fixCropBox(bool fixsized);
    void drawCropBoxInternalLines(bool drawable);
    void setCropBoxLine(const int & widthcount,const int& heightcount);

    void setCropBoxShape(CropBoxShape shape = CropBoxShape::Rect);
    CropBoxShape getCropBoxShape();

    void setZoomMode(ZoomMode mode = ZoomMode::Free);
    void setEnableKeyPressEvent(bool enabled);

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void drawBorder(QPainter &painter);
    void drawPoints(QPainter &painter);
    void drawInternalLines(QPainter &painter);
    void drawSizeText(QPainter &painter);

    void setDirection(QPoint point);

    void handleMove(QPoint mouse_globalpos);
    void handleResize(QPoint mouse_parentpos);

    void handleResizeUp(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeDown(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeLeft(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeRight(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeRightTop(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeRightBottom(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeLeftTop(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);
    void handleResizeLeftBottom(QPoint &valid_point, QRect &rectNew, const QWidget *parent_widget);

    inline int judgePosition(int origin, int min, int max);

private:
    CropBoxShape m_shape;
    ZoomMode m_zoomMode;
    ZoomMode m_keyPressZoomMode;

    int m_minWidth;
    int m_minHeight;

    double m_heightwidthRatio;
    double m_ratioMinWidth;
    double m_ratioMinHeight;

    bool m_bFixSized;
    bool m_bDrawInternalLines;
    int m_widthCount;
    int m_heightCount;

    bool m_bMovingFlag;
    Direction m_curDirec;
    QPoint m_dragPosition;
};

#endif // CROPBOX_H
