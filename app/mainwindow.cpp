#include "mainwindow.h"
#include <QtWidgets>
#include <QDockWidget>
#include <QGraphicsItem>
#include <QMdiSubWindow>
#include <QUndoStack>
#include <QFile>
#include <QTextStream>
#include "drawobj.h"
#include "commands.h"
#include "materials.h"
#include "context.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    undoStack = new QUndoStack(this);
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->setAttribute(Qt::WA_QuitOnClose, false);

    // QDockWidget *dock = new QDockWidget(this);
    // addDockWidget(Qt::RightDockWidgetArea, dock);

    // dock->setWidget(undoView);

    m_view = createGraphicsView();
    setCentralWidget(m_view);

    setWindowTitle(tr("Qt Drawing"));
    setUnifiedTitleAndToolBarOnMac(true);

    createActions();
    createMenus();
    createToolbars();

    setCurrentFile(QString());
 /*
    m_posInfo = new QLabel(tr("x,y"));
    m_posInfo->setMinimumSize(m_posInfo->sizeHint());
    m_posInfo->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(m_posInfo);
*/
    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(dataChanged()));
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(updateActions()));
    m_timer.start(500);

}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save As"), this);
    saveAsAct->setShortcuts(QKeySequence::Save);
    saveAsAct->setStatusTip(tr("Save file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    // create align actions
    rightAct   = new QAction(QIcon(":/icons/align_right.png"),tr("right"),this);
    leftAct    = new QAction(QIcon(":/icons/align_left.png"),tr("left"),this);
    vCenterAct = new QAction(QIcon(":/icons/align_vcenter.png"),tr("vcenter"),this);
    hCenterAct = new QAction(QIcon(":/icons/align_hcenter.png"),tr("hcenter"),this);
    upAct      = new QAction(QIcon(":/icons/align_top.png"),tr("top"),this);
    downAct    = new QAction(QIcon(":/icons/align_bottom.png"),tr("bottom"),this);
    horzAct    = new QAction(QIcon(":/icons/align_horzeven.png"),tr("Horizontal"),this);
    vertAct    = new QAction(QIcon(":/icons/align_verteven.png"),tr("vertical"),this);
    heightAct  = new QAction(QIcon(":/icons/align_height.png"),tr("height"),this);
    widthAct   = new QAction(QIcon(":/icons/align_width.png"),tr("width"),this);
    allAct     = new QAction(QIcon(":/icons/align_all.png"),tr("width and height"),this);

    bringToFrontAct = new QAction(QIcon(":/icons/bringtofront.png"),tr("bring to front"),this);
    sendToBackAct   = new QAction(QIcon(":/icons/sendtoback.png"),tr("send to back"),this);
    groupAct        = new QAction(QIcon(":/icons/group.png"),tr("group"),this);
    unGroupAct        = new QAction(QIcon(":/icons/ungroup.png"),tr("ungroup"),this);

    connect(bringToFrontAct,SIGNAL(triggered()),this,SLOT(on_actionBringToFront_triggered()));
    connect(sendToBackAct,SIGNAL(triggered()),this,SLOT(on_actionSendToBack_triggered()));
    connect(rightAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(leftAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(vCenterAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(hCenterAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(upAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(downAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));

    connect(horzAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(vertAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(heightAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(widthAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));
    connect(allAct,SIGNAL(triggered()),this,SLOT(on_aglin_triggered()));

    connect(groupAct,SIGNAL(triggered()),this,SLOT(on_group_triggered()));
    connect(unGroupAct,SIGNAL(triggered()),this,SLOT(on_unGroup_triggered()));


    //create draw actions
    selectAct = new QAction(QIcon(":/icons/arrow.png"),tr("select tool"),this);
    selectAct->setCheckable(true);
    lineAct = new QAction(QIcon(":/icons/line.png"),tr("line tool"),this);
    lineAct->setCheckable(true);
    rectAct = new QAction(QIcon(":/icons/rectangle.png"),tr("rect tool"),this);
    rectAct->setCheckable(true);
    roundRectAct =  new QAction(QIcon(":/icons/roundrect.png"),tr("roundrect tool"),this);
    roundRectAct->setCheckable(true);
    ellipseAct = new QAction(QIcon(":/icons/ellipse.png"),tr("ellipse tool"),this);
    ellipseAct->setCheckable(true);
    polygonAct = new QAction(QIcon(":/icons/polygon.png"),tr("polygon tool"),this);
    polygonAct->setCheckable(true);
    polylineAct = new QAction(QIcon(":/icons/polyline.png"),tr("polyline tool"),this);
    polylineAct->setCheckable(true);
    bezierAct= new QAction(QIcon(":/icons/bezier.png"),tr("bezier tool"),this);
    bezierAct->setCheckable(true);

    rotateAct = new QAction(QIcon(":/icons/rotate.png"),tr("rotate tool"),this);
    rotateAct->setCheckable(true);

    drawActionGroup = new QActionGroup(this);
    drawActionGroup->addAction(selectAct);
    drawActionGroup->addAction(lineAct);
    drawActionGroup->addAction(rectAct);
    drawActionGroup->addAction(roundRectAct);
    drawActionGroup->addAction(ellipseAct);
    drawActionGroup->addAction(polygonAct);
    drawActionGroup->addAction(polylineAct);
    drawActionGroup->addAction(bezierAct);
    drawActionGroup->addAction(rotateAct);
    selectAct->setChecked(true);

    connect(selectAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(lineAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(rectAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(roundRectAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(ellipseAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(polygonAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(polylineAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(bezierAct,SIGNAL(triggered()),this,SLOT(addShape()));
    connect(rotateAct,SIGNAL(triggered()),this,SLOT(addShape()));

    mergeAct = new QAction(QIcon(":/icons/merge.png"),tr("merge"),this);
    mergeAct->setCheckable(true);
    replacesOldAct = new QAction(QIcon(":/icons/newreplacesold.png"),tr("new replaces old"),this);
    replacesOldAct->setCheckable(true);
    replacesNewAct = new QAction(QIcon(":/icons/oldreplacesnew.png"),tr("old replaces new"),this);
    replacesNewAct->setCheckable(true);
    overlapsOldAct = new QAction(QIcon(":/icons/newoverlapsold.png"),tr("new overlaps old"),this);
    overlapsOldAct->setCheckable(true);
    overlapsNewAct = new QAction(QIcon(":/icons/oldoverlapsnew.png"),tr("old overlaps new"),this);
    overlapsNewAct->setCheckable(true);

    booleanActionGroup = new QActionGroup(this);
    booleanActionGroup->addAction(mergeAct);
    booleanActionGroup->addAction(replacesOldAct);
    booleanActionGroup->addAction(replacesNewAct);
    booleanActionGroup->addAction(overlapsOldAct);
    booleanActionGroup->addAction(overlapsNewAct);
    replacesOldAct->setChecked(true);

    connect(mergeAct,SIGNAL(triggered()),this,SLOT(setBooleanOperation()));
    connect(replacesOldAct,SIGNAL(triggered()),this,SLOT(setBooleanOperation()));
    connect(replacesNewAct,SIGNAL(triggered()),this,SLOT(setBooleanOperation()));
    connect(overlapsOldAct,SIGNAL(triggered()),this,SLOT(setBooleanOperation()));
    connect(overlapsNewAct,SIGNAL(triggered()),this,SLOT(setBooleanOperation()));

    deleteAct = new QAction(tr("&Delete"), this);
    deleteAct->setShortcut(QKeySequence::Delete);

    undoAct = undoStack->createUndoAction(this,tr("undo"));
    undoAct->setIcon(QIcon(":/icons/undo.png"));
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = undoStack->createRedoAction(this,tr("redo"));
    redoAct->setIcon(QIcon(":/icons/redo.png"));
    redoAct->setShortcuts(QKeySequence::Redo);

    zoomInAct = new QAction(QIcon(":/icons/zoomin.png"),tr("zoomIn"),this);
    zoomOutAct = new QAction(QIcon(":/icons/zoomout.png"),tr("zoomOut"),this);

    copyAct = new QAction(QIcon(":/icons/copy.png"),tr("copy"),this);
    copyAct->setShortcut(QKeySequence::Copy);

    pasteAct = new QAction(QIcon(":/icons/paste.png"),tr("paste"),this);
    pasteAct->setShortcut(QKeySequence::Paste);
    pasteAct->setEnabled(false);
    cutAct = new QAction(QIcon(":/icons/cut.png"),tr("cut"),this);
    cutAct->setShortcut(QKeySequence::Cut);

    connect(copyAct,SIGNAL(triggered()),this,SLOT(on_copy()));
    connect(pasteAct,SIGNAL(triggered()),this,SLOT(on_paste()));
    connect(cutAct,SIGNAL(triggered()),this,SLOT(on_cut()));

    connect(zoomInAct , SIGNAL(triggered()),this,SLOT(zoomIn()));
    connect(zoomOutAct , SIGNAL(triggered()),this,SLOT(zoomOut()));
    connect(deleteAct, SIGNAL(triggered()), this, SLOT(deleteItem()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    funcAct = new QAction(tr("test"),this);
    connect(funcAct,SIGNAL(triggered()),this,SLOT(on_func_test_triggered()));

}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(deleteAct);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);

    QMenu *toolMenu = menuBar()->addMenu(tr("&Tools"));
    QMenu *shapeTool = new QMenu("&Shape");
    shapeTool->addAction(selectAct);
    shapeTool->addAction(rectAct);
    // shapeTool->addAction(roundRectAct);
    // shapeTool->addAction(ellipseAct);
    shapeTool->addAction(polygonAct);
    // shapeTool->addAction(polylineAct);
    // shapeTool->addAction(bezierAct);
    // shapeTool->addAction(rotateAct);
    toolMenu->addMenu(shapeTool);

    QMenu *booleanMenu = new QMenu("Boolean");
    booleanMenu->addAction(mergeAct);
    booleanMenu->addAction(replacesOldAct);
    booleanMenu->addAction(replacesNewAct);
    booleanMenu->addAction(overlapsOldAct);
    booleanMenu->addAction(overlapsNewAct);
    toolMenu->addMenu(booleanMenu);

    // QMenu *alignMenu = new QMenu("Align");
    // alignMenu->addAction(rightAct);
    // alignMenu->addAction(leftAct);
    // alignMenu->addAction(hCenterAct);
    // alignMenu->addAction(vCenterAct);
    // alignMenu->addAction(upAct);
    // alignMenu->addAction(downAct);
    // alignMenu->addAction(horzAct);
    // alignMenu->addAction(vertAct);
    // alignMenu->addAction(heightAct);
    // alignMenu->addAction(widthAct);
    // alignMenu->addAction(allAct);
    // toolMenu->addMenu(alignMenu);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(separatorAct);
    helpMenu->addAction(funcAct);
}

void MainWindow::createToolbars()
{
    // create edit toolbar
    editToolBar = addToolBar(tr("edit"));
    editToolBar->setIconSize(QSize(24,24));
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(cutAct);

    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);

    editToolBar->addAction(zoomInAct);
    editToolBar->addAction(zoomOutAct);

    QComboBox *comboBox = new QComboBox();
    auto mats = Materials::instance();
    for (int i=0; i < mats->getMaterials().count(); i++){
        auto mat = mats->getMaterials()[i];
        comboBox->addItem(mat.name);
        comboBox->setItemData(i, mat.color, Qt::BackgroundColorRole);
    }
    editToolBar->addWidget(comboBox);
    connect(comboBox, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index){
        QColor color = comboBox->itemData(index, Qt::BackgroundColorRole).value<QColor>();
        QString rgb = QString("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
        comboBox->setStyleSheet("QComboBox { background-color: "+rgb+" }");
        auto ctx = Context::instance();
        ctx->setCurrentMaterial(comboBox->itemText(index));
    });
    comboBox->setCurrentIndex(-1);
    comboBox->setCurrentIndex(0);

    // create draw toolbar
    drawToolBar = addToolBar(tr("drawing"));
    drawToolBar->setIconSize(QSize(24,24));
    drawToolBar->addAction(selectAct);
    // drawToolBar->addAction(lineAct);
    drawToolBar->addAction(rectAct);
    // drawToolBar->addAction(roundRectAct);
    // drawToolBar->addAction(ellipseAct);
    drawToolBar->addAction(polygonAct);
    // drawToolBar->addAction(polylineAct);
    // drawToolBar->addAction(bezierAct);
    // drawToolBar->addAction(rotateAct);

    booleanToolBar = addToolBar(tr("boolean"));
    booleanToolBar->setIconSize(QSize(24,24));
    booleanToolBar->addAction(mergeAct);
    booleanToolBar->addAction(replacesOldAct);
    booleanToolBar->addAction(replacesNewAct);
    booleanToolBar->addAction(overlapsOldAct);
    booleanToolBar->addAction(overlapsNewAct);

    // create align toolbar
    alignToolBar = addToolBar(tr("align"));
    alignToolBar->setIconSize(QSize(24,24));
    // alignToolBar->addAction(upAct);
    // alignToolBar->addAction(downAct);
    // alignToolBar->addAction(rightAct);
    // alignToolBar->addAction(leftAct);
    // alignToolBar->addAction(vCenterAct);
    // alignToolBar->addAction(hCenterAct);

    // alignToolBar->addAction(horzAct);
    // alignToolBar->addAction(vertAct);
    // alignToolBar->addAction(heightAct);
    // alignToolBar->addAction(widthAct);
    // alignToolBar->addAction(allAct);

    alignToolBar->addAction(bringToFrontAct);
    alignToolBar->addAction(sendToBackAct);
    // alignToolBar->addAction(groupAct);
    // alignToolBar->addAction(unGroupAct);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    setWindowModified(false);

    QString shownName = m_currentFile;
    if (m_currentFile.isEmpty())
        shownName = "untitled.xml";
    setWindowFilePath(shownName);

    if (m_currentFile.isEmpty())
        return;
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        m_changed = false;
        setCurrentFile(QString());
        m_view->newFile();
        // TODO: refresh view and data
        undoStack->clear();
        m_view->scene()->clear();
        m_view->setModified(false);
    }
}

void MainWindow::open()
{
    if (!maybeSave()) {
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        // TODO: checkout fileName is not same as current file
        if (loadFile(fileName))
            statusBar()->showMessage(tr("File loaded"), 2000);
    }
}

bool MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Drawing"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    undoStack->clear();
    m_view->scene()->clear();
    m_view->setModified(false);
    // TODO: refresh view and data

    QXmlStreamReader xml(&file);
    if (xml.readNextStartElement()) {
        if ( xml.name() == tr("canvas"))
        {
            int width = xml.attributes().value(tr("width")).toInt();
            int height = xml.attributes().value(tr("height")).toInt();
            m_view->scene()->setSceneRect(0,0,width,height);
            m_view->loadCanvas(&xml);
        }
    }

    // setCurrentFile(fileName);
    qDebug()<<xml.errorString();
    if (xml.error() != QXmlStreamReader::NoError)
    {
        return false;
    }

    setCurrentFile(fileName);
    // TODO: load other data
    // refresh window

    m_changed = false;
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setDefaultSuffix(".xml");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Drawing"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE qdraw>");
    xml.writeStartElement("canvas");
    xml.writeAttribute("width",QString("%1").arg(m_view->scene()->width()));
    xml.writeAttribute("height",QString("%1").arg(m_view->scene()->height()));

    for (QGraphicsItem *item : m_view->scene()->items()) {
        AbstractShape * ab = qgraphicsitem_cast<AbstractShape*>(item);
        QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
        if ( ab &&!qgraphicsitem_cast<SizeHandleRect*>(ab) && !g ){
            ab->saveToXml(&xml);
        }
    }
    
    // TODO: save other data
    xml.writeEndElement();
    xml.writeEndDocument();

    setCurrentFile(fileName);
    m_changed = false;
    m_view->setModified(false);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

bool MainWindow::save()
{
    if (m_currentFile.isEmpty()) {
        return saveAs();
    }
    else {
        return saveFile(m_currentFile);
    }
}

bool MainWindow::maybeSave()
{
    // TODO: use m_changed
    if (!m_view->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

DrawView *MainWindow::createGraphicsView()
{
    DrawScene *scene = new DrawScene(this);

    QRectF rc = QRectF(0 , 0 , 800, 600);

    scene->setSceneRect(rc);
    qDebug()<<rc.bottomLeft()<<rc.size() << rc.topLeft();

    connect(scene, SIGNAL(selectionChanged()),
            this, SLOT(itemSelected()));

    connect(scene,SIGNAL(itemAdded(QGraphicsItem*)),
            this, SLOT(itemAdded(QGraphicsItem*)));
    connect(scene,SIGNAL(itemMoved(QGraphicsItem*,QPointF)),
            this,SLOT(itemMoved(QGraphicsItem*,QPointF)));
    connect(scene,SIGNAL(itemRotate(QGraphicsItem*,qreal)),
            this,SLOT(itemRotate(QGraphicsItem*,qreal)));

    connect(scene,SIGNAL(itemResize(QGraphicsItem* , int , const QPointF&)),
            this,SLOT(itemResize(QGraphicsItem*,int,QPointF)));

    connect(scene,SIGNAL(itemControl(QGraphicsItem* , int , const QPointF&,const QPointF&)),
            this,SLOT(itemControl(QGraphicsItem*,int,QPointF,QPointF)));

    DrawView *view = new DrawView(scene);
    scene->setView(view);
    connect(view,SIGNAL(positionChanged(int,int)),this,SLOT(positionChanged(int,int)));

    view->setRenderHint(QPainter::Antialiasing);
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    //view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // move orign point to leftbottom
    view->setTransform(view->transform().scale(1,-1));

    scene->setBackgroundBrush(Qt::darkGray);
    return view;
}

void MainWindow::addShape()
{
    if ( sender() == selectAct )
        DrawTool::c_drawShape = selection;
    else if (sender() == lineAct )
        DrawTool::c_drawShape = line;
    else if ( sender() == rectAct )
        DrawTool::c_drawShape = rectangle;
    else if ( sender() == roundRectAct )
        DrawTool::c_drawShape = roundrect;
    else if ( sender() == ellipseAct )
        DrawTool::c_drawShape = ellipse ;
    else if ( sender() == polygonAct )
        DrawTool::c_drawShape = polygon;
    else if ( sender() == bezierAct )
        DrawTool::c_drawShape = bezier ;
    else if (sender() == rotateAct )
        DrawTool::c_drawShape = rotation;
    else if (sender() == polylineAct )
        DrawTool::c_drawShape = polyline;

    if ( sender() != selectAct && sender() != rotateAct ){
        m_view->scene()->clearSelection();
    }
    if ( sender() != selectAct){
        m_view->scene()->clearSelection();
    }
}

void MainWindow::setBooleanOperation()
{
    Context *context = Context::instance();
    if (sender() == mergeAct)
        context->setBooleanOperation(MergeOper);
    if (sender() == replacesOldAct)
        context->setBooleanOperation(NewReplacesOldOper);
    if (sender() == replacesNewAct)
        context->setBooleanOperation(OldReplacesNewOper);
    if (sender() == overlapsOldAct)
        context->setBooleanOperation(NewOverlpasOldOper);
    if (sender() == overlapsNewAct)
        context->setBooleanOperation(OldOverlpasNewOper);
}

void MainWindow::updateActions()
{
    Context *context = Context::instance();
    QGraphicsScene * scene = m_view->scene();

    selectAct->setEnabled(true);
    lineAct->setEnabled(true);
    rectAct->setEnabled(true);
    roundRectAct->setEnabled(true);
    ellipseAct->setEnabled(true);
    bezierAct->setEnabled(true);
    rotateAct->setEnabled(true);
    polygonAct->setEnabled(true);
    polylineAct->setEnabled(true);

    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);

    selectAct->setChecked(DrawTool::c_drawShape == selection);
    lineAct->setChecked(DrawTool::c_drawShape == line);
    rectAct->setChecked(DrawTool::c_drawShape == rectangle);
    roundRectAct->setChecked(DrawTool::c_drawShape == roundrect);
    ellipseAct->setChecked(DrawTool::c_drawShape == ellipse);
    bezierAct->setChecked(DrawTool::c_drawShape == bezier);
    rotateAct->setChecked(DrawTool::c_drawShape == rotation);
    polygonAct->setChecked(DrawTool::c_drawShape == polygon);
    polylineAct->setChecked(DrawTool::c_drawShape == polyline );

    mergeAct->setChecked(context->boolOperation() == MergeOper);
    replacesOldAct->setChecked(context->boolOperation() == NewReplacesOldOper);
    replacesNewAct->setChecked(context->boolOperation() == OldReplacesNewOper);
    overlapsOldAct->setChecked(context->boolOperation() == NewOverlpasOldOper);
    overlapsNewAct->setChecked(context->boolOperation() == OldOverlpasNewOper);

    undoAct->setEnabled(undoStack->canUndo());
    redoAct->setEnabled(undoStack->canRedo());

    bringToFrontAct->setEnabled(scene->selectedItems().count() > 0);
    sendToBackAct->setEnabled(scene->selectedItems().count() > 0);
    groupAct->setEnabled( scene->selectedItems().count() > 0);
    unGroupAct->setEnabled(scene->selectedItems().count() > 0 &&
                              dynamic_cast<GraphicsItemGroup*>( scene->selectedItems().first()));

    leftAct->setEnabled(scene->selectedItems().count() > 1);
    rightAct->setEnabled(scene->selectedItems().count() > 1);
    leftAct->setEnabled(scene->selectedItems().count() > 1);
    vCenterAct->setEnabled(scene->selectedItems().count() > 1);
    hCenterAct->setEnabled(scene->selectedItems().count() > 1);
    upAct->setEnabled(scene->selectedItems().count() > 1);
    downAct->setEnabled(scene->selectedItems().count() > 1);

    heightAct->setEnabled(scene->selectedItems().count() > 1);
    widthAct->setEnabled(scene->selectedItems().count() > 1);
    allAct->setEnabled(scene->selectedItems().count()>1);
    horzAct->setEnabled(scene->selectedItems().count() > 2);
    vertAct->setEnabled(scene->selectedItems().count() > 2);

    copyAct->setEnabled(scene->selectedItems().count() > 0);
    cutAct->setEnabled(scene->selectedItems().count() > 0);
}

void MainWindow::itemSelected()
{
    QGraphicsScene * scene = m_view->scene();

    if ( scene->selectedItems().count() > 0
         && scene->selectedItems().first()->isSelected())
    {
        QGraphicsItem *item = scene->selectedItems().first();
    }
    return;
}

void MainWindow::itemMoved(QGraphicsItem *item, const QPointF &oldPosition)
{
    Q_UNUSED(item);
    m_view->setModified(true);

    if ( item ){
        QUndoCommand *moveCommand = new MoveShapeCommand(item, oldPosition);
        undoStack->push(moveCommand);
    }else{
        QUndoCommand *moveCommand = new MoveShapeCommand(m_view->scene(), oldPosition);
        undoStack->push(moveCommand);
    }
}

void MainWindow::itemAdded(QGraphicsItem *item)
{
    m_view->setModified(true);

    QUndoCommand *addCommand = new AddShapeCommand(item, item->scene());
    undoStack->push(addCommand);

    QList<QGraphicsItem *> IntersectsItems = item->collidingItems();
    Context *context = Context::instance();
    BooleanOperation oper = context->boolOperation();

    AbstractShape *shape = qgraphicsitem_cast<AbstractShape*>(item);
    for (auto it:IntersectsItems)
    {
        auto path = shape->path();
        path = item->mapToScene(path);
        AbstractShape *itShape = qgraphicsitem_cast<AbstractShape*>(it);
        if (!itShape || qgraphicsitem_cast<SizeHandleRect*>(itShape))
            continue;
        auto itPath = itShape->path();
        itPath = it->mapToScene(itPath);

        if (oper == NewReplacesOldOper)
        {
            itPath = itPath.subtracted(path);
            itPath.closeSubpath();
            itPath = it->mapFromScene(itPath);
            itShape->setPath(itPath);
        } else if (oper == OldReplacesNewOper)
        {
            path = path.subtracted(itPath);
            path.closeSubpath();
            path = item->mapFromScene(path);
            shape->setPath(path);
        }
    }
}

void MainWindow::itemRotate(QGraphicsItem *item, const qreal oldAngle)
{
    m_view->setModified(true);

    QUndoCommand *rotateCommand = new RotateShapeCommand(item , oldAngle);
    undoStack->push(rotateCommand);
}

void MainWindow::itemResize(QGraphicsItem *item, int handle, const QPointF& scale)
{
    m_view->setModified(true);

    QUndoCommand *resizeCommand = new ResizeShapeCommand(item ,handle, scale );
    undoStack->push(resizeCommand);
}

void MainWindow::itemControl(QGraphicsItem *item, int handle, const QPointF & newPos ,const QPointF &lastPos_)
{
    m_view->setModified(true);

    QUndoCommand *controlCommand = new ControlShapeCommand(item ,handle, newPos, lastPos_ );
    undoStack->push(controlCommand);
}

void MainWindow::deleteItem()
{
    qDebug()<<"deleteItem";

    QGraphicsScene * scene = m_view->scene();
    m_view->setModified(true);

    if (scene->selectedItems().isEmpty())
        return;

    QUndoCommand *deleteCommand = new RemoveShapeCommand(scene);
    undoStack->push(deleteCommand);

}

void MainWindow::on_actionBringToFront_triggered()
{
    QGraphicsScene * scene = m_view->scene();

    if (scene->selectedItems().isEmpty())
        return;
    m_view->setModified(true);

    QGraphicsItem *selectedItem = scene->selectedItems().first();

    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();
    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == GraphicsItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);


}
void MainWindow::on_actionSendToBack_triggered()
{
    QGraphicsScene * scene = m_view->scene();

    if (scene->selectedItems().isEmpty())
        return;

    m_view->setModified(true);

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == GraphicsItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}

void MainWindow::on_aglin_triggered()
{
    DrawScene * scene =dynamic_cast<DrawScene*>(m_view->scene());

    m_view->setModified(true);

    if ( sender() == rightAct ){
        scene->align(RIGHT_ALIGN);
    }else if ( sender() == leftAct){
        scene->align(LEFT_ALIGN);
    }else if ( sender() == upAct ){
        scene->align(UP_ALIGN);
    }else if ( sender() == downAct ){
        scene->align(DOWN_ALIGN);
    }else if ( sender() == vCenterAct ){
        scene->align(VERT_ALIGN);
    }else if ( sender() == hCenterAct){
        scene->align(HORZ_ALIGN);
    }else if ( sender() == heightAct )
        scene->align(HEIGHT_ALIGN);
    else if ( sender()==widthAct )
        scene->align(WIDTH_ALIGN);
    else if ( sender() == horzAct )
        scene->align(HORZEVEN_ALIGN);
    else if ( sender() == vertAct )
        scene->align(VERTEVEN_ALIGN);
    else if ( sender () == allAct )
        scene->align(ALL_ALIGN);
}

void MainWindow::zoomIn()
{
    m_view->zoomIn();
}

void MainWindow::zoomOut()
{
    m_view->zoomOut();
}

void MainWindow::on_group_triggered()
{
    DrawScene * scene = dynamic_cast<DrawScene*>(m_view->scene());

    //QGraphicsItemGroup
    QList<QGraphicsItem *> selectedItems = scene->selectedItems();
    // Create a new group at that level
    if ( selectedItems.count() < 1) return;
    GraphicsItemGroup *group = scene->createGroup(selectedItems);
    QUndoCommand *groupCommand = new GroupShapeCommand(group,scene);
    undoStack->push(groupCommand);
}

void MainWindow::on_unGroup_triggered()
{
    QGraphicsScene * scene = m_view->scene();

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    GraphicsItemGroup * group = dynamic_cast<GraphicsItemGroup*>(selectedItem);
    if ( group ){
        QUndoCommand *unGroupCommand = new UnGroupShapeCommand(group,scene);
        undoStack->push(unGroupCommand);
    }
}

struct GmshPoint {
    int id;
    QPointF p;
};

struct GmshLine {
    int id;
    QList<int> pointIds;
};

void MainWindow::on_func_test_triggered()
{
    QString str;
    str += "SetFactory(\"OpenCASCADE\");\n";
    str += "Mesh.MeshSizeMin = 5;\n";
    str += "Mesh.MeshSizeMax = 10;\n\n";

    int pointId = 1;
    int lineId = 1;
    int surfaceId = 1;
    QMap<QGraphicsItem *, int> itemIdMap;
    QGraphicsScene * scene = m_view->scene();
    QList<QGraphicsItem *> items;
    for (auto item: scene->items())
    {
        AbstractShape *shape = qgraphicsitem_cast<AbstractShape*>(item);
        if (!shape || qgraphicsitem_cast<SizeHandleRect*>(shape))
            continue;
        items.append(item);
    }
    for (auto item: items)
    {
        AbstractShape *shape = qgraphicsitem_cast<AbstractShape*>(item);
        auto path = shape->path();
        path.closeSubpath();
        path = item->mapToScene(path);
        qDebug() << path;
        QList<GmshPoint> points;
        QList<QString> lineIds;
        QList<GmshLine> lines;
        for (int i=0; i<path.elementCount(); i++)
        {
            auto e = path.elementAt(i);
            // if (e.isMoveTo())
            // if (e.isLineTo())
            QPointF p = QPointF(e);
            GmshPoint point;
            point.id = pointId++;
            point.p = p;
            points.append(point);
            if (i == 0)
            {
                continue;
            }
            GmshLine line;
            line.id = lineId++;
            line.pointIds.append(pointId-2);
            line.pointIds.append(pointId-1);
            lineIds.append(QString::number(line.id));
            lines.append(line);

            lineId++;  // gmsh add extra line for close
        }
        for (auto p:points)
        {
            str += QString("Point(%1) = {%2, %3, 0, 10};\n").arg(p.id).arg(p.p.x()).arg(p.p.y());
        }
        for (auto l:lines)
        {
            str += QString("Line(%1) = {%2, %3};\n").arg(l.id).arg(l.pointIds[0]).arg(l.pointIds[1]);
        }
        itemIdMap[item] = surfaceId;
        int curveLoopId = surfaceId++;
        str += QString("Curve Loop(%1) = {%2};\n").arg(curveLoopId).arg(lineIds.join(","));
        str += QString("Plane Surface(%1) = {%2};\n\n").arg(curveLoopId).arg(curveLoopId);
    }

    for (auto item : items)
    {
        QList<QString> intersects;
        for (auto it : items)
        {
            if (it == item)
                continue;
            if (item->collidesWithItem(it))
            {
                intersects.append(QString::number(itemIdMap[it]));
            }
        }
        if (intersects.isEmpty())
            continue;

        QString fmt = "BooleanFragments { Surface{%1}; Delete; }{ Surface{%2};Delete; };\n";
        str += QString(fmt).arg(itemIdMap[item]).arg(intersects.join(","));
    }

    QString filename = "gmsh.geo";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QTextStream stream(&file);
    stream << str << endl;
}

void MainWindow::on_copy()
{
    QGraphicsScene * scene = m_view->scene();

    ShapeMimeData * data = new ShapeMimeData( scene->selectedItems() );
    QApplication::clipboard()->setMimeData(data);
}

void MainWindow::on_paste()
{
    QGraphicsScene * scene = m_view->scene();

    QMimeData * mp = const_cast<QMimeData *>(QApplication::clipboard()->mimeData()) ;
    ShapeMimeData * data = dynamic_cast< ShapeMimeData*>( mp );
    if ( data ){
        scene->clearSelection();
        foreach (QGraphicsItem * item , data->items() ) {
            AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
            QGraphicsItem * copy = sp->duplicate();
            if ( copy ){
                copy->setSelected(true);
                copy->moveBy(10,10);
                QUndoCommand *addCommand = new AddShapeCommand(copy, scene);
                undoStack->push(addCommand);
            }
        }
    }
}

void MainWindow::on_cut()
{
    QGraphicsScene * scene = m_view->scene();

    QList<QGraphicsItem *> copylist ;
    foreach (QGraphicsItem *item , scene->selectedItems()) {
        AbstractShape *sp = qgraphicsitem_cast<AbstractShape*>(item);
        QGraphicsItem * copy = sp->duplicate();
        if ( copy )
            copylist.append(copy);
    }
    QUndoCommand *deleteCommand = new RemoveShapeCommand(scene);
    undoStack->push(deleteCommand);
    if ( copylist.count() > 0 ){
        ShapeMimeData * data = new ShapeMimeData( copylist );
        QApplication::clipboard()->setMimeData(data);
    }
}

void MainWindow::dataChanged()
{
    pasteAct->setEnabled(true);
}

void MainWindow::positionChanged(int x, int y)
{
   char buf[255];
   sprintf(buf,"%d,%d",x,y);
   statusBar()->showMessage(buf);
}

void MainWindow::about()
{

}
