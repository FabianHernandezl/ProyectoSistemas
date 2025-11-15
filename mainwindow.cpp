#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QHeaderView>
#include <QPainter>
#include <QDebug>
#include <QTableWidgetItem>
#include "persistence_manager.h" // Asegúrate de incluir tu clase de persistencia
#include <QJsonObject>
#include <QCloseEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>

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
    background_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/fondo/factory2_background.png");
    conveyorBelt_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/cinta_transportadora/conveyor_belt.png");
    box_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/caja/caja.png");
    box2_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/caja/caja2.png");

    worker1_ = QPixmap("//home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker1.png");
    worker2_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker2.png");
    worker3_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker3.png");
    worker4_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker4.png");
    worker5_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker5.png");

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

    // Inicializamos la animación (sin empezar automáticamente)
    // animationManager_.startAnimations();
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
    // Inicia la producción y la animación cuando se presiona el botón de Start
    controller_.startProduction();
    animationManager_.startAnimations();  // Inicia la animación
}

void MainWindow::on_btnPause_clicked()
{
    controller_.pauseProduction();
    animationManager_.stopAnimations();
    PersistenceManager::saveTable(currentRows_);
}

void MainWindow::updateProcessedCount(int v)
{
    ui->barProductos->setValue(v);
}

void MainWindow::updateActiveWorkers(int v)
{
    ui->barRecursos->setValue(v);
}

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

    // Crear objeto JSON para esta fila
    QJsonObject obj;
    obj["station"] = station;
    obj["productId"] = productId;
    obj["state"] = state;
    obj["time"] = time;

    // Guardar en la lista en memoria
    currentRows_.append(obj);

    // Persistir todo en JSON
    PersistenceManager::saveTable(currentRows_);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Guardar los datos al cerrar la aplicación
    PersistenceManager::saveTable(currentRows_);
    event->accept();
}

void MainWindow::on_btnExit_clicked()
{
    qApp->quit();
}

void MainWindow::on_btnDelete_clicked()
{
    // Mostrar cuadro de diálogo para eliminar
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Eliminar Producción",
                                  "¿Desea eliminar toda la producción o solo un proceso?",
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        clearAllProduction();
    } else if (reply == QMessageBox::No) {
        deleteSpecificProcess();
    }
}

void MainWindow::clearAllProduction()
{
    ui->tblProcesses->setRowCount(0);  // Eliminar todas las filas
    currentRows_ = QJsonArray();  // Reiniciar los datos en memoria
    PersistenceManager::saveTable(currentRows_);  // Guardar
    QMessageBox::information(this, "Eliminación Completa", "Toda la producción ha sido eliminada.");
}

void MainWindow::deleteSpecificProcess()
{
    // Crear un cuadro de lista para eliminar un proceso específico
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QListWidget *listWidget = new QListWidget(dialog);
    for (int row = 0; row < ui->tblProcesses->rowCount(); ++row) {
        QString processInfo = ui->tblProcesses->item(row, 1)->text();
        listWidget->addItem("Producto " + processInfo);
    }
    layout->addWidget(listWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    dialog->setLayout(layout);
    dialog->exec();

    if (dialog->result() == QDialog::Accepted) {
        QString selectedProcess = listWidget->currentItem()->text();
        bool processFound = false;
        int productId = selectedProcess.split(" ").last().toInt();

        for (int row = 0; row < ui->tblProcesses->rowCount(); ++row) {
            if (ui->tblProcesses->item(row, 1)->text().toInt() == productId) {
                ui->tblProcesses->removeRow(row);

                // Eliminar también del JSON
                for (int i = 0; i < currentRows_.size(); ++i) {
                    QJsonObject obj = currentRows_[i].toObject();
                    if (obj["productId"].toInt() == productId) {
                        currentRows_.removeAt(i);
                        PersistenceManager::saveTable(currentRows_);
                        break;
                    }
                }

                processFound = true;
                QMessageBox::information(this, "Proceso Eliminado", "El proceso seleccionado ha sido eliminado.");
                break;
            }
        }

        if (!processFound) {
            QMessageBox::warning(this, "Error", "Este proceso no existe.");
        }
    }

    delete dialog;
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
