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
    QMessageBox msg(this);
    msg.setWindowTitle("Eliminar Producción");
    msg.setText("¿Qué desea eliminar?");

    QPushButton *btnAll = msg.addButton("Toda la producción", QMessageBox::AcceptRole);
    QPushButton *btnOne = msg.addButton("Un proceso", QMessageBox::ActionRole);
    QPushButton *btnCancel = msg.addButton("Cancelar", QMessageBox::RejectRole);

    msg.exec();

    if (msg.clickedButton() == btnAll) {
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

            return;

    } else if (msg.clickedButton() == btnOne) {
        deleteSpecificProcess();
    } else if (msg.clickedButton() == btnCancel) {
        // No hacer nada (cancelado)
        return;
    }
}

void MainWindow::clearAllProduction()
{
    // Limpiar tabla
    ui->tblProcesses->setRowCount(0);

    // Limpiar logs
    ui->txtLog->clear();

    // Reiniciar contador de productos
    controller_.resetProductCounter();

    // Limpiar JSON
    currentRows_ = QJsonArray();
    PersistenceManager::saveTable(currentRows_);

    QMessageBox::information(this,
                             "Producción eliminada",
                             "Toda la producción ha sido eliminada correctamente.");
}

void MainWindow::deleteSpecificProcess()
{
    // 1. Construir lista de IDs únicos a partir de la tabla
    QSet<int> idsUnicos;

    for (int row = 0; row < ui->tblProcesses->rowCount(); ++row) {
        bool ok = false;
        int id = ui->tblProcesses->item(row, 1)->text().toInt(&ok);
        if (ok) {
            idsUnicos.insert(id);
        }
    }

    if (idsUnicos.isEmpty()) {
        QMessageBox::information(this, "Sin procesos",
                                 "No hay procesos disponibles para eliminar.");
        return;
    }

    // 2. Crear diálogo modal
    QDialog dialog(this);
    dialog.setWindowTitle("Eliminar proceso");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QListWidget *listWidget = new QListWidget(&dialog);
    for (int id : idsUnicos) {
        listWidget->addItem(QString("Producto %1").arg(id));
    }
    layout->addWidget(listWidget);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
    layout->addWidget(buttonBox);

    // Conectar botones a aceptar / cancelar
    QObject::connect(buttonBox, &QDialogButtonBox::accepted,
                     &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected,
                     &dialog, &QDialog::reject);

    dialog.setLayout(layout);

    // 3. Mostrar diálogo
    if (dialog.exec() != QDialog::Accepted) {
        // Usuario canceló
        return;
    }

    // 4. Verificar selección
    QListWidgetItem *item = listWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Sin selección",
                             "No se ha seleccionado ningún proceso.");
        return;
    }

    int productId = item->text().split(' ').last().toInt();

    // 5. Verificar si el producto ya fue finalizado
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
        QMessageBox::warning(this, "No se puede eliminar",
                             "El proceso seleccionado ya fue finalizado.");
        return;
    }

    // 6. Eliminar TODAS las filas de ese producto en la tabla
    for (int row = ui->tblProcesses->rowCount() - 1; row >= 0; --row) {
        int idFila = ui->tblProcesses->item(row, 1)->text().toInt();
        if (idFila == productId) {
            ui->tblProcesses->removeRow(row);
        }
    }

    // 7. Eliminar también del JSON en memoria
    for (int i = currentRows_.size() - 1; i >= 0; --i) {
        QJsonObject obj = currentRows_[i].toObject();
        if (obj["productId"].toInt() == productId) {
            currentRows_.removeAt(i);
        }
    }
    PersistenceManager::saveTable(currentRows_);

    QMessageBox::information(this, "Proceso eliminado",
                             QString("Se eliminó el proceso del producto %1.").arg(productId));
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
