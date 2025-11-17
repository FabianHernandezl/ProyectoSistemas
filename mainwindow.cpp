#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QHeaderView>
#include <QPainter>
#include <QDebug>
#include <QTableWidgetItem>
#include "persistence_manager.h"
#include <QJsonObject>
#include <QCloseEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <QSet>

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

    //box_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/caja/caja.png");

    box2_ = QPixmap("/home/fahern/Descargas/ProyectoSistemas/resources/caja/caja2.png");

    worker1Anim_.setFrames("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker1_a.png",
                           "/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker1_b.png");

    worker2Anim_.setFrames("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker2_a.png",
                           "/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker2_b.png");

    worker3Anim_.setFrames("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker3_a.png",
                           "/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker3_b.png");

    worker4Anim_.setFrames("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker4_a.png",
                           "/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker4_b.png");

    worker5Anim_.setFrames("/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker5_a.png",
                           "/home/fahern/Descargas/ProyectoSistemas/resources/personajes/worker5_b.png");

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

    //
    // -------------------------
    //  CONFIG BARRAS
    // -------------------------
    //
    ui->barProductos->setRange(0, 100);
    ui->barProductos->setValue(0);

    ui->barRecursos->setRange(0, 5);
    ui->barRecursos->setValue(0);

    //
    // -------------------------
    //  CONECTAR LOG
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
    //  ESTADÍSTICAS
    // -------------------------
    //
    connect(&controller_, &ProductionController::updateProcessedCount,
            this, &MainWindow::updateProcessedCount);

    connect(&controller_, &ProductionController::updateActiveWorkers,
            this, &MainWindow::updateActiveWorkers);

    //
    // -------------------------
    //  EVENTOS DE ESTACIÓN
    // -------------------------
    //
    connect(&controller_, &ProductionController::processEvent,
            this, &MainWindow::onProcessEvent);

    //
    // -------------------------
    //  ANIMACIÓN
    // -------------------------
    //
    connect(&animationManager_, &AnimationManager::positionChanged,
            this, &MainWindow::onAnimationUpdated);

    connect(&worker1Anim_, &AnimationWorkerManager::frameChanged, this, QOverload<>::of(&MainWindow::update));
    connect(&worker2Anim_, &AnimationWorkerManager::frameChanged, this, QOverload<>::of(&MainWindow::update));
    connect(&worker3Anim_, &AnimationWorkerManager::frameChanged, this, QOverload<>::of(&MainWindow::update));
    connect(&worker4Anim_, &AnimationWorkerManager::frameChanged, this, QOverload<>::of(&MainWindow::update));
    connect(&worker5Anim_, &AnimationWorkerManager::frameChanged, this, QOverload<>::of(&MainWindow::update));

    connect(&animationManager_, &AnimationManager::pause1Reached, &worker1Anim_, &AnimationWorkerManager::start);
    connect(&animationManager_, &AnimationManager::pause2Reached, &worker2Anim_, &AnimationWorkerManager::start);
    connect(&animationManager_, &AnimationManager::pause3Reached, &worker3Anim_, &AnimationWorkerManager::start);
    connect(&animationManager_, &AnimationManager::pause4Reached, &worker4Anim_, &AnimationWorkerManager::start);
    connect(&animationManager_, &AnimationManager::pause5Reached, &worker5Anim_, &AnimationWorkerManager::start);

    connect(&animationManager_, &AnimationManager::stop1Reached, &worker1Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::stop2Reached, &worker2Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::stop3Reached, &worker3Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::stop4Reached, &worker4Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::stop5Reached, &worker5Anim_, &AnimationWorkerManager::stop);

    connect(&animationManager_, &AnimationManager::allWorkersStop, &worker1Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::allWorkersStop, &worker2Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::allWorkersStop, &worker3Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::allWorkersStop, &worker4Anim_, &AnimationWorkerManager::stop);
    connect(&animationManager_, &AnimationManager::allWorkersStop, &worker5Anim_, &AnimationWorkerManager::stop);


    //
    // -------------------------
    //  CARGA DE PERSISTENCIA
    // -------------------------
    //
    currentRows_ = PersistenceManager::loadEvents();

    for (auto row : currentRows_) {
        QJsonObject obj = row.toObject();

        int r = ui->tblProcesses->rowCount();
        ui->tblProcesses->insertRow(r);

        ui->tblProcesses->setItem(r, 0, new QTableWidgetItem(obj["station"].toString()));
        ui->tblProcesses->setItem(r, 1, new QTableWidgetItem(QString::number(obj["productId"].toInt())));
        ui->tblProcesses->setItem(r, 2, new QTableWidgetItem(obj["state"].toString()));
        ui->tblProcesses->setItem(r, 3, new QTableWidgetItem(obj["time"].toString()));
    }

    QString msgBoxStyle = R"(
QMessageBox {
    background-color: #ffe8b5;
    border: 3px solid #d4a95a;
    border-radius: 15px;
    font-family: "Segoe UI", Arial, sans-serif;
    font-size: 16px;
}

QLabel {
    color: #5a4123;
    font-family: "Segoe UI", Arial, sans-serif;
    font-size: 16px;
}

QPushButton {
    background-color: #e6b85c;
    border: 2px solid #a87c3a;
    border-radius: 8px;
    padding: 6px 14px;
    font-size: 14px;
    font-family: "Segoe UI", Arial, sans-serif;
    color: #5a4123;
    font-weight: bold;
}

QPushButton:hover {
    background-color: #f0c97c;
}

QPushButton:pressed {
    background-color: #d4a95a;
}
)";
    qApp->setStyleSheet(qApp->styleSheet() + msgBoxStyle);

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

    // TRABAJADOR 1 ANIMADO
    painter.drawPixmap(
        200, 200,
        worker1Anim_.currentFrame().scaled(300, 400, Qt::KeepAspectRatio,Qt::SmoothTransformation)
        );

    // TRABAJADOR 2 ANIMADO
    painter.drawPixmap(
        435, 240,
        worker2Anim_.currentFrame().scaled(300, 400, Qt::KeepAspectRatio,Qt::SmoothTransformation)
        );

    // TRABAJADOR 3 ANIMADO
    painter.drawPixmap(
        700, 250,
        worker3Anim_.currentFrame().scaled(300, 400, Qt::KeepAspectRatio,Qt::SmoothTransformation)
        );

    // TRABAJADOR 4 ANIMADO
    painter.drawPixmap(
        900, 300,
        worker4Anim_.currentFrame().scaled(400, 600, Qt::KeepAspectRatio,Qt::SmoothTransformation)
        );

    // TRABAJADOR 5 ANIMADO
    painter.drawPixmap(
        1200, 175,
        worker5Anim_.currentFrame().scaled(300, 500, Qt::KeepAspectRatio,Qt::SmoothTransformation)
        );

    int beltWidth = conveyorBelt_.width();
    QPixmap stretchedBelt = conveyorBelt_.scaled(beltWidth, beltHeight, Qt::IgnoreAspectRatio);
    painter.drawPixmap(-40, beltY, stretchedBelt);

    if (!box_.isNull())
        painter.drawPixmap(250, beltY - 20, box_.scaled(200, 200));

    painter.drawPixmap(width() - 480, beltY + 50, box2_.scaled(150, 150));

    animationManager_.render(painter);

    QMainWindow::paintEvent(event);
}


//
// -------------------------
//  BOTÓN START — MENÚ AVANZADO
// -------------------------
void MainWindow::on_btnStart_clicked()
{
    QMessageBox menu(this);
    menu.setWindowTitle("Opciones de inicio");
    menu.setText("Seleccione una acción:");

    QPushButton *btnStartProduction = menu.addButton("▶️ Iniciar toda la producción", QMessageBox::AcceptRole);
    QPushButton *btnStartAnimations = menu.addButton("▶️ Iniciar solo animaciones", QMessageBox::ActionRole);
    QPushButton *btnStartStation = menu.addButton("▶️ Iniciar estación específica", QMessageBox::ActionRole);
    QPushButton *btnStartMaint = menu.addButton("🧹 Iniciar hilos de mantenimiento", QMessageBox::ActionRole);

    QPushButton *btnCancel = menu.addButton("Cancelar", QMessageBox::RejectRole);

    menu.exec();

    if (menu.clickedButton() == btnStartProduction) {
        controller_.startProduction();
        animationManager_.startAnimations();
    }
    else if (menu.clickedButton() == btnStartAnimations) {
        animationManager_.startAnimations();
    }else if (menu.clickedButton() == btnStartStation) {

        if (controller_.stationsEmpty()) {
            QMessageBox::warning(this,
                                 "Error",
                                 "Debe iniciar la producción antes de seleccionar una estación.");
            return;
        }

        controller_.startSpecificStationDialog(this);
    }
    else if (menu.clickedButton() == btnStartMaint) {
        controller_.startMaintenanceThreads();
    }
}


//
// -------------------------
//  BOTÓN PAUSE — MENÚ AVANZADO
// -------------------------
void MainWindow::on_btnPause_clicked()
{
    QMessageBox menu(this);
    menu.setWindowTitle("Opciones de pausa");
    menu.setText("Seleccione qué desea pausar:");

    QPushButton *btnPauseAll = menu.addButton("⏸️ Pausar toda la producción", QMessageBox::ActionRole);
    QPushButton *btnPauseAnimations = menu.addButton("⏸️ Pausar solo animaciones", QMessageBox::ActionRole);
    QPushButton *btnPauseStation = menu.addButton("⏸️ Pausar estación específica", QMessageBox::ActionRole);
    QPushButton *btnPauseMaint = menu.addButton("⏸️ Pausar hilos de mantenimiento", QMessageBox::ActionRole);

    QPushButton *btnCancel = menu.addButton("Cancelar", QMessageBox::RejectRole);

    menu.exec();

    if (menu.clickedButton() == btnPauseAll) {
        controller_.pauseProduction();
        animationManager_.stopAnimations();
    }
    else if (menu.clickedButton() == btnPauseAnimations) {
        animationManager_.stopAnimations();
    }
    else if (menu.clickedButton() == btnPauseStation) {

        if (controller_.stationsEmpty()) {
            QMessageBox::warning(this,
                                 "Error",
                                 "Debe iniciar la producción antes de pausar estaciones.");
            return;
        }

        controller_.pauseSpecificStationDialog(this);
    }
    else if (menu.clickedButton() == btnPauseMaint) {
        controller_.pauseMaintenanceThreads();
    }
}



//
// -------------------------
//  BOTÓN DELETE — MENÚ AVANZADO
// -------------------------
void MainWindow::on_btnDelete_clicked()
{
    QMessageBox menu(this);
    menu.setWindowTitle("Eliminar / Detener");
    menu.setText("Seleccione una acción:");

    QPushButton *btnDeleteAll = menu.addButton("🗑️ Eliminar toda la producción", QMessageBox::DestructiveRole);
    QPushButton *btnDeleteOne = menu.addButton("🗑️ Eliminar proceso específico", QMessageBox::ActionRole);
    QPushButton *btnStopOne = menu.addButton("⛔ Detener estación específica", QMessageBox::ActionRole);
    QPushButton *btnStopAll = menu.addButton("⛔ Detener todas las estaciones (POISON PILL)", QMessageBox::DestructiveRole);

    QPushButton *btnCancel = menu.addButton("Cancelar", QMessageBox::RejectRole);
    menu.exec();

    if (menu.clickedButton() == btnDeleteAll) {
        // Confirmación adicional
        QMessageBox::StandardButton confirm =
            QMessageBox::warning(
                this,
                "Confirmar eliminación",
                "¿Está seguro que desea eliminar toda la producción?\n"
                "Esta acción no se puede deshacer.",
                QMessageBox::Yes | QMessageBox::Cancel
                );

        if (confirm == QMessageBox::Yes) {
            clearAllProduction();
        } else {
            QMessageBox::information(this,
                                     "Cancelado",
                                     "La eliminación fue cancelada.");
        }

        return;    }
    else if (menu.clickedButton() == btnDeleteOne) {
        deleteSpecificProcess();
    }
    else if (menu.clickedButton() == btnStopOne) {

        if (controller_.stationsEmpty()) {
            QMessageBox::warning(this,
                                 "Error",
                                 "Debe iniciar la producción antes de detener estaciones.");
            return;
        }

        controller_.stopSpecificStationDialog(this);
    }
    else if (menu.clickedButton() == btnStopAll) {
        controller_.stopAllStations();
    }

}



//
// -------------------------
//  BOTÓN EXIT — MENÚ AVANZADO
// -------------------------
void MainWindow::on_btnExit_clicked()
{
    QMessageBox menu(this);
    menu.setWindowTitle("Salir del sistema");
    menu.setText("¿Seguro que desea salir?");

    QPushButton *btnSaveExit = menu.addButton("💾 Guardar y salir", QMessageBox::AcceptRole);
    QPushButton *btnForceExit = menu.addButton("⚠️ Salida forzada", QMessageBox::DestructiveRole);
    QPushButton *btnCancel = menu.addButton("Cancelar", QMessageBox::RejectRole);

    menu.exec();

    if (menu.clickedButton() == btnSaveExit) {
        PersistenceManager::saveEvents(currentRows_);
        qApp->quit();
    }
    else if (menu.clickedButton() == btnForceExit) {
        qApp->quit();
    }
}

//
// -------------------------
//  ANIMACIÓN
// -------------------------
void MainWindow::onAnimationUpdated(const QPoint &pos)
{
    animPosition_ = pos;
    update();
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

    QJsonObject obj;
    obj["station"] = station;
    obj["productId"] = productId;
    obj["state"] = state;
    obj["time"] = time;

    currentRows_.append(obj);
    PersistenceManager::saveTable(currentRows_);
}

void MainWindow::clearAllProduction()
{
    ui->tblProcesses->setRowCount(0);
    ui->txtLog->clear();

    controller_.resetProductCounter();

    currentRows_ = QJsonArray();
    PersistenceManager::saveTable(currentRows_);

    QMessageBox::information(this, "Producción eliminada",
                             "Toda la producción ha sido eliminada.");
}

void MainWindow::deleteSpecificProcess()
{
    // --- 1. Obtener lista única de IDs ---
    QSet<int> idsUnicos;

    for (int row = 0; row < ui->tblProcesses->rowCount(); ++row) {
        bool ok = false;
        int id = ui->tblProcesses->item(row, 1)->text().toInt(&ok);
        if (ok) idsUnicos.insert(id);
    }

    if (idsUnicos.isEmpty()) {
        QMessageBox::information(this, "Sin procesos",
                                 "No hay procesos disponibles para eliminar.");
        return;
    }

    // --- 2. Crear diálogo para escoger el ID ---
    QDialog dialog(this);
    dialog.setWindowTitle("Eliminar proceso específico");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QListWidget *listWidget = new QListWidget(&dialog);
    for (int id : idsUnicos)
        listWidget->addItem(QString("Producto %1").arg(id));

    layout->addWidget(listWidget);

    QDialogButtonBox *btnBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        &dialog
        );
    layout->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // --- 3. Ejecutar diálogo ---
    if (dialog.exec() != QDialog::Accepted) return;

    QListWidgetItem *item = listWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Sin selección",
                             "Debe seleccionar un proceso para eliminar.");
        return;
    }

    int productId = item->text().split(' ').last().toInt();

    // --- 4. Verificar si el proceso ya fue finalizado ---
    bool finalizado = false;

    for (int row = 0; row < ui->tblProcesses->rowCount(); ++row) {
        int idFila = ui->tblProcesses->item(row, 1)->text().toInt();
        QString estado = ui->tblProcesses->item(row, 2)->text();

        if (idFila == productId && estado == "Finalizado") {
            finalizado = true;
            break;
        }
    }

    if (finalizado) {
        QMessageBox::warning(this,
                             "No se puede eliminar",
                             "El proceso seleccionado ya fue finalizado.");
        return;
    }

    // --- 5. Eliminar todas las filas del ID ---
    for (int row = ui->tblProcesses->rowCount() - 1; row >= 0; --row) {
        int idFila = ui->tblProcesses->item(row, 1)->text().toInt();
        if (idFila == productId) {
            ui->tblProcesses->removeRow(row);
        }
    }

    // --- 6. Eliminar del JSON ---
    for (int i = currentRows_.size() - 1; i >= 0; --i) {
        QJsonObject obj = currentRows_[i].toObject();
        if (obj["productId"].toInt() == productId) {
            currentRows_.removeAt(i);
        }
    }

    PersistenceManager::saveTable(currentRows_);

    // --- 7. Confirmación ---
    QMessageBox::information(
        this,
        "Proceso eliminado",
        QString("Se eliminó correctamente el proceso del producto %1.")
            .arg(productId)
        );
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    PersistenceManager::saveTable(currentRows_);
    event->accept();
}
