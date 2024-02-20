#include "commands.h"
#include <QDebug>
MoveShapeCommand::MoveShapeCommand(QGraphicsScene *graphicsScene, const QPointF &delta, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myItem = 0;
    myItems = graphicsScene->selectedItems();
    myGraphicsScene = graphicsScene;
    myDelta = delta;
    bMoved = true;
}

MoveShapeCommand::MoveShapeCommand(QGraphicsItem * item, const QPointF &delta, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = 0;
    myItem = item;
    myDelta = delta;
    bMoved = true;
}

void MoveShapeCommand::undo()
{
    if ( myItem ) {
        myItem->moveBy(-myDelta.x(),-myDelta.y());
        myItem->scene()->update();
    } else if( myItems.count() > 0 ){
        foreach (QGraphicsItem *item, myItems) {
           item->moveBy(-myDelta.x(),-myDelta.y());
        }
        myGraphicsScene->update();
    }
    setText(QObject::tr("Undo Move %1,%2")
        .arg(-myDelta.x()).arg(-myDelta.y()));
    bMoved = false;
}

void MoveShapeCommand::redo()
{
    if ( !bMoved ){
        if ( myItem ){
            myItem->moveBy(myDelta.x(),myDelta.y());
            myItem->scene()->update();
        }else if( myItems.count() > 0 ){
            foreach (QGraphicsItem *item, myItems) {
               item->moveBy(myDelta.x(),myDelta.y());
            }
            myGraphicsScene->update();
        }
    }
    setText(QObject::tr("Redo Move %1,%2")
        .arg(myDelta.x()).arg(myDelta.y()));
}

RemoveShapeCommand::RemoveShapeCommand(QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    myGraphicsScene = scene;
    items = myGraphicsScene->selectedItems();
}

RemoveShapeCommand::~RemoveShapeCommand()
{

}

void RemoveShapeCommand::undo()
{
    foreach (QGraphicsItem *item, items) {
        myGraphicsScene->addItem(item);
    }
    myGraphicsScene->update();
    setText(QObject::tr("Undo Delete %1").arg(items.count()));
}

void RemoveShapeCommand::redo()
{
    foreach (QGraphicsItem *item, items) {
        myGraphicsScene->removeItem(item);
    }
    myGraphicsScene->update();
    setText(QObject::tr("Redo Delete %1").arg(items.count()));
}

AddShapeCommand::AddShapeCommand(QGraphicsItem *item,
                       QGraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    static int itemCount = 0;

    myGraphicsScene = scene;
    myDiagramItem = item;
    initialPosition = item->pos();
    ++itemCount;
}

AddShapeCommand::~AddShapeCommand()
{
    if (!myDiagramItem->scene())
        delete myDiagramItem;
}

void AddShapeCommand::undo()
{
    myGraphicsScene->removeItem(myDiagramItem);
    myGraphicsScene->update();
    setText(QObject::tr("Undo Add %1")
        .arg(createCommandString(myDiagramItem, initialPosition)));

}

void AddShapeCommand::redo()
{
    if ( myDiagramItem->scene() == NULL )
        myGraphicsScene->addItem(myDiagramItem);
    myDiagramItem->setPos(initialPosition);
    myGraphicsScene->update();
    setText(QObject::tr("Redo Add %1")
        .arg(createCommandString(myDiagramItem, initialPosition)));

}

QString createCommandString(QGraphicsItem *item, const QPointF &pos)
{
    return QObject::tr("Item at (%1, %2)")
        .arg(pos.x()).arg(pos.y());
}

// ResizeShapeCommand::ResizeShapeCommand(QGraphicsItem *item,
//                                        int handle,
//                                        const QPointF& scale,
//                                        QUndoCommand *parent)
// {
//     myItem = item;
//     handle_ = handle;
//     scale_  = QPointF(scale) ;
//     opposite_ = Handle_None;
//     bResized = true;       
// }

// void ResizeShapeCommand::undo()
// {

//     int handle = handle_;

//     AbstractShape * item = qgraphicsitem_cast<AbstractShape*>(myItem);
//     if ( item ){
//         if ( Handle_None != opposite_ ){
//             handle = opposite_;
//         }

//         item->stretch(handle,1./scale_.x(),1./scale_.y(),item->opposite(handle));
//         item->updateCoordinate();
//         item->update();
//     }
//     bResized = false;
//     setText(QObject::tr("Undo Resize %1,%2 ,handle:%3")
//         .arg(1./scale_.x(),8,'f',2).arg(1./scale_.y(),8,'f',2).arg(handle));

// }

// void ResizeShapeCommand::redo()
// {
//     int handle = handle_;
//     if ( !bResized ){
//         AbstractShape * item = qgraphicsitem_cast<AbstractShape*>(myItem);
//         if ( item ){
//             item->stretch(handle,scale_.x(),scale_.y(),item->opposite(handle));
//             item->updateCoordinate();
//             item->update();
//         }
//     }
//     setText(QObject::tr("Redo Resize %1,%2 ,handle:%3")
//         .arg(scale_.x(),8,'f',2).arg(scale_.y(),8,'f',2).arg(handle));

// }
// bool ResizeShapeCommand::mergeWith(const QUndoCommand *command)
// {
//     if (command->id() != ResizeShapeCommand::Id )
//         return false;

//     const ResizeShapeCommand *cmd = static_cast<const ResizeShapeCommand *>(command);

//     QGraphicsItem *item = cmd->myItem;

//     if (myItem != item)
//         return false;

//     if ( cmd->handle_ != handle_ )
//         return false;

//     AbstractShape * ab = qgraphicsitem_cast<AbstractShape*>(item);

//     opposite_ = ab->swapHandle(cmd->handle_,cmd->scale_);

//     handle_ = cmd->handle_;
//     scale_ = cmd->scale_;
//     setText(QObject::tr(" mergeWith Resize %1,%2,%3,%4")
//         .arg(scale_.x(),8,'f',2).arg(scale_.y() ,8,'f',2).arg(handle_).arg(opposite_));

//     return true;
// }

ControlShapeCommand::ControlShapeCommand(QGraphicsItem *item,
                                       int handle,
                                       const QPointF& newPos,
                                       const QPointF& lastPos,
                                       QUndoCommand *parent)
{
    myItem = item;
    handle_ = handle;
    lastPos_  = QPointF(lastPos) ;
    newPos_ = QPointF(newPos);
    bControled = true;
}

void ControlShapeCommand::undo()
{

    AbstractShape * item = qgraphicsitem_cast<AbstractShape*>(myItem);
    if ( item ){
        item->control(handle_,lastPos_);
        item->updateCoordinate();
        item->update();
    }
    bControled = false;
    setText(QObject::tr("Undo Control %1,%2")
        .arg(lastPos_.x()).arg(lastPos_.y()));

}

void ControlShapeCommand::redo()
{
    if ( !bControled ){
        AbstractShape * item = qgraphicsitem_cast<AbstractShape*>(myItem);
        if ( item ){
            item->control(handle_,newPos_);
            item->updateCoordinate();
            item->update();
        }
    }
    setText(QObject::tr("Redo Control %1,%2")
        .arg(newPos_.x()).arg(newPos_.y()));

}
bool ControlShapeCommand::mergeWith(const QUndoCommand *command)
{
    if (command->id() != ControlShapeCommand::Id )
        return false;

    const ControlShapeCommand *cmd = static_cast<const ControlShapeCommand *>(command);
    QGraphicsItem *item = cmd->myItem;

    if (myItem != item )
        return false;
    if ( cmd->handle_ != handle_ )
        return false;
    handle_ = cmd->handle_;
    lastPos_ = cmd->lastPos_;
    newPos_  = cmd->newPos_;
    setText(QObject::tr(" mergeWith Control %1,%2,%3")
        .arg(newPos_.x()).arg(newPos_.y()).arg(handle_));

    return true;
}
