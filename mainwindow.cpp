#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QHeaderView>
#include <QPainter>
#include <QDebug>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //
    // -------------------------
    //  CARGA DE IMÁGENES
    // -------------------------
    //
    background_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/fondo/factory2_background.png");
    conveyorBelt_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/cinta_transportadora/conveyor_belt.png");
    box_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/caja/caja.png");
    box2_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/caja/caja2.png");

    worker1_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/personajes/worker1.png");
    worker2_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/personajes/worker2.png");
    worker3_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/personajes/worker3.png");
    worker4_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/personajes/worker4.png");
    worker5_ = QPixmap("/home/fahern/Descargas/IF4001_ProductionLine/resources/personajes/worker5.png");

    //
    // -------------------------
    //  CONFIG TABLA DE PROCESOS
    // -------------------------
    //
    ui->tblProcesses->setColumnCount(4);
    ui->tblProcesses->setHorizontalHeaderLabels(QStringList()
                                                << "Estación"
                                                << "Producto ID"
                                                << "Estado"
                                                << "Tiempo");

    ui->tblProcesses->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tblProcesses->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tblProcesses->setSelectionMode(QAbstractItemView::NoSelection);

    ui->tblProcesses->setStyleSheet(
        "QTableWidget { background: rgba(255,255,255,180); font-size: 14px; }"
        "QHeaderView::section { background: #c0d8f0; font-weight: bold; }"
        );

    //
    // -------------------------
    //  CONFIG BARRAS
    // -------------------------
    //
    ui->barProductos->setRange(0, 100);
    ui->barProductos->setValue(0);
    ui->barProductos->setStyleSheet(
        "QProgressBar { background: rgba(255,255,255,180); border-radius: 5px; }"
        "QProgressBar::chunk { background-color: #4aa3ff; }"
        );

    ui->barRecursos->setRange(0, 5);
    ui->barRecursos->setValue(0);
    ui->barRecursos->setStyleSheet(
        "QProgressBar { background: rgba(255,255,255,180); border-radius: 5px; }"
        "QProgressBar::chunk { background-color: #ff944d; }"
        );

    //
    // -------------------------
    //  CONECTAR LOG DEL SISTEMA
    // -------------------------
    //
    connect(&controller_, &ProductionController::logMessage,
            this, [this](const QString &msg){
                ui->txtLog->appendPlainText(msg);
                ui->txtLog->verticalScrollBar()->setValue(
                    ui->txtLog->verticalScrollBar()->maximum());
            });

    //
    // -------------------------
    //  CONECTAR ESTADÍSTICAS
    // -------------------------
    //
    connect(&controller_, &ProductionController::updateProcessedCount,
            this, &MainWindow::updateProcessedCount);

    connect(&controller_, &ProductionController::updateActiveWorkers,
            this, &MainWindow::updateActiveWorkers);

    //
    // -------------------------
    //  CONECTAR EVENTOS DE ESTACIÓN
    // -------------------------
    //
    connect(&controller_, &ProductionController::processEvent,
            this, &MainWindow::onProcessEvent);

    //
    // -------------------------
    //  🔥 CONECTAR ANIMACIÓN
    // -------------------------
    //
    connect(&animationManager_, &AnimationManager::positionChanged,
            this, &MainWindow::onAnimationUpdated);

    animationManager_.startAnimations();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
// -------------------------
//      DIBUJO GRÁFICO
// -------------------------
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, width(), height(), background_);

    int beltHeight = conveyorBelt_.height() * 0.25;
    int beltY = height() - beltHeight - 200;

    painter.drawPixmap(200, 200,
                       worker1_.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    painter.drawPixmap(435, 240,
                       worker2_.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    painter.drawPixmap(700, 250,
                       worker3_.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    painter.drawPixmap(900, 300,
                       worker4_.scaled(400, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    painter.drawPixmap(1200, 175,
                       worker5_.scaled(300, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    int beltWidth = conveyorBelt_.width();
    QPixmap stretchedBelt = conveyorBelt_.scaled(beltWidth, beltHeight, Qt::IgnoreAspectRatio);
    painter.drawPixmap(-40, beltY, stretchedBelt);

    if (!box_.isNull()) {
        QPixmap scaled = box_.scaled(200, 200, Qt::KeepAspectRatio);
        painter.drawPixmap(250, beltY - 20, scaled);
    }

    if (!box2_.isNull()) {
        QPixmap scaled = box2_.scaled(150, 150, Qt::KeepAspectRatio);
        painter.drawPixmap(width() - 480, beltY + 50, scaled);
    }

    //
    // 🔥 DIBUJA LA ANIMACIÓN DE LA CAJA
    //
    animationManager_.render(painter);


    QMainWindow::paintEvent(event);
}

//
// -------------------------
//  BOTONES
// -------------------------
void MainWindow::on_btnStart_clicked()
{
    controller_.startProduction();
    animationManager_.startAnimations();
}

void MainWindow::on_btnStop_clicked()
{
    controller_.stopProduction();
    animationManager_.stopAnimations();
}

//
// -------------------------
//  ACTUALIZAR ESTADÍSTICAS
// -------------------------
void MainWindow::updateProcessedCount(int v)
{
    ui->barProductos->setValue(v);
}

void MainWindow::updateActiveWorkers(int v)
{
    ui->barRecursos->setValue(v);
}

//
// -------------------------
//  LLENAR TABLA DE PROCESOS
// -------------------------
void MainWindow::onProcessEvent(const QString &station,
                                int productId,
                                const QString &state,
                                const QString &time)
{
    int row = ui->tblProcesses->rowCount();
    ui->tblProcesses->insertRow(row);

    ui->tblProcesses->setItem(row, 0, new QTableWidgetItem(station));
    ui->tblProcesses->setItem(row, 1, new QTableWidgetItem(QString::number(productId)));
    ui->tblProcesses->setItem(row, 2, new QTableWidgetItem(state));
    ui->tblProcesses->setItem(row, 3, new QTableWidgetItem(time));

    ui->tblProcesses->scrollToBottom();
}

//
// -------------------------
//  🔥 Recibir actualización de animación
// -------------------------
void MainWindow::onAnimationUpdated(const QPoint &pos)
{
    animPosition_ = pos;
    update();  // Fuerza repintado
}
