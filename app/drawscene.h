#ifndef DRAWSCENE
#define DRAWSCENE

#include <QGraphicsScene>
#include "drawtool.h"
#include "drawobj.h"

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QGraphicsTextItem;
class QColor;
class QKeyEvent;
class QPainter;
QT_END_NAMESPACE

class GridTool
{
public:
    GridTool(const QSize &grid = QSize(3200,2400) , const QSize & space = QSize(20,20) );
    void paintGrid(QPainter *painter,const QRect & rect );
protected:
    QSize m_sizeGrid;
    QSize m_sizeGridSpace;
};

class DrawScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit DrawScene(QObject *parent = 0);
    ~DrawScene();
    void setView(QGraphicsView * view ) { m_view = view ; }
    QGraphicsView * view() { return m_view; }
    void mouseEvent(QGraphicsSceneMouseEvent *mouseEvent );
signals:
    void itemMoved( QGraphicsItem * item , const QPointF & oldPosition );
    void itemAdded(QGraphicsItem * item );
    void itemResize(QGraphicsItem * item , int handle , const QPointF& scale );
    void itemControl(QGraphicsItem * item , int handle , const QPointF & newPos , const QPointF& lastPos_ );

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvet) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    QGraphicsView * m_view;

    qreal m_dx;
    qreal m_dy;
    bool  m_moved;
    GridTool *m_grid;
};

#endif // DRAWSCENE

