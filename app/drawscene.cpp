#include "drawscene.h"
#include<QGraphicsSceneMouseEvent>
#include<QGraphicsRectItem>
#include <QDebug>
#include <QKeyEvent>
#include "drawobj.h"
#include <vector>
#include <QPainter>


GridTool::GridTool(const QSize & grid , const QSize & space )
    :m_sizeGrid(grid)
    ,m_sizeGridSpace(20,20)
{
}


void GridTool::paintGrid(QPainter *painter, const QRect &rect)
{
    QColor c(Qt::darkCyan);
    QPen p(c);
    p.setStyle(Qt::DashLine);
    p.setWidthF(0.2);
    painter->setPen(p);

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing,false);

    painter->fillRect(rect,Qt::white);
    for (int x=rect.left() ;x <rect.right()  ;x+=(int)(m_sizeGridSpace.width())) {
        painter->drawLine(x,rect.top(),x,rect.bottom());

    }
    for (int y=rect.top();y<rect.bottom() ;y+=(int)(m_sizeGridSpace.height()))
    {
        painter->drawLine(rect.left(),y,rect.right(),y);
    }
    p.setStyle(Qt::SolidLine);
    p.setColor(Qt::black);
    painter->drawLine(rect.right(),rect.top(),rect.right(),rect.bottom());
    painter->drawLine(rect.left(),rect.bottom(),rect.right(),rect.bottom());

    //draw shadow
//    QColor c1(Qt::black);
//    painter->fillRect(QRect(rect.right()+1,rect.top()+2,2,rect.height()),c1.dark(200));
//    painter->fillRect(QRect(rect.left()+2,rect.bottom()+2,rect.width(),2),c1.dark(200));

    painter->restore();
}

DrawScene::DrawScene(QObject *parent)
    :QGraphicsScene(parent)
{
    m_view = NULL;
    m_dx=m_dy=0;
    m_grid = new GridTool();
    QGraphicsItem * item = addRect(QRectF(0,0,0,0));
    item->setAcceptHoverEvents(true);

}

DrawScene::~DrawScene()
{
    delete m_grid;
}

void DrawScene::mouseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    switch( mouseEvent->type() ){
    case QEvent::GraphicsSceneMousePress:
        QGraphicsScene::mousePressEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseMove:
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        break;
    }
}

void DrawScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter,rect);
    painter->fillRect(sceneRect(),Qt::white);
    if( m_grid ){
        m_grid->paintGrid(painter,sceneRect().toRect());
    }
}

void DrawScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
        tool->mousePressEvent(mouseEvent,this);
}

void DrawScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
        tool->mouseMoveEvent(mouseEvent,this);
}

void DrawScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
        tool->mouseReleaseEvent(mouseEvent,this);
}

void DrawScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvet)
{
    DrawTool * tool = DrawTool::findTool( DrawTool::c_drawShape );
    if ( tool )
        tool->mouseDoubleClickEvent(mouseEvet,this);

}

void DrawScene::keyPressEvent(QKeyEvent *e)
{
    qreal dx=0,dy=0;
    m_moved = false;
    switch( e->key())
    {
    case Qt::Key_Up:
        dx = 0;
        dy = -1;
        m_moved = true;
        break;
    case Qt::Key_Down:
        dx = 0;
        dy = 1;
        m_moved = true;
        break;
    case Qt::Key_Left:
        dx = -1;
        dy = 0;
        m_moved = true;
        break;
    case Qt::Key_Right:
        dx = 1;
        dy = 0;
        m_moved = true;
        break;
    }
    m_dx += dx;
    m_dy += dy;
    if ( m_moved )
    foreach (QGraphicsItem *item, selectedItems()) {
       item->moveBy(dx,dy);
    }
    QGraphicsScene::keyPressEvent(e);
}

void DrawScene::keyReleaseEvent(QKeyEvent *e)
{
    if (m_moved && selectedItems().count()>0)
    emit itemMoved(NULL,QPointF(m_dx,m_dy));
    m_dx=m_dy=0;
    QGraphicsScene::keyReleaseEvent(e);
}

