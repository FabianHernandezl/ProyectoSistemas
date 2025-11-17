#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include "productioncontroller.h"
#include "animationmanager.h"
#include "animationworkermanager.h"
#include "persistence_manager.h"  // Asegúrate de que esté incluida para persistencia
#include <QJsonArray>
#include <QJsonObject>
#include <QCloseEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;  // No se toca, tu código sigue igual
    void closeEvent(QCloseEvent *event) override;  // Para guardar datos al cerrar

private slots:
    // Botones
    void on_btnStart_clicked();   // Botón para iniciar la producción
    void on_btnPause_clicked();   // Botón para pausar la producción
    void on_btnExit_clicked();    // Botón para salir de la aplicación
    void on_btnDelete_clicked();  // Botón para eliminar la producción

    // Barras de estadísticas
    void updateProcessedCount(int v);  // Actualiza el contador de productos procesados
    void updateActiveWorkers(int v);   // Actualiza el número de trabajadores activos

    void onProcessEvent(const QString &station,
                        int productId,
                        const QString &state,
                        const QString &time);  // Evento para los procesos

    // 🔥 Recibimos posición animada
    void onAnimationUpdated(const QPoint &pos);

    // Métodos para manejar la eliminación de la producción
    void clearAllProduction();         // Eliminar toda la producción
    void deleteSpecificProcess();      // Eliminar un proceso específico

private:
    Ui::MainWindow *ui;
    ProductionController controller_;
    AnimationManager animationManager_;  // Gestor de animación
    AnimationWorkerManager worker1Anim_;
    AnimationWorkerManager worker2Anim_;
    AnimationWorkerManager worker3Anim_;
    AnimationWorkerManager worker4Anim_;
    AnimationWorkerManager worker5Anim_;

    // Imágenes
    QPixmap background_;
    QPixmap conveyorBelt_;
    QPixmap box_;
    QPixmap box2_;
    QPixmap worker1_;
    QPixmap worker2_;
    QPixmap worker3_;
    QPixmap worker4_;
    QPixmap worker5_;

    QJsonArray currentRows_;  // Para almacenar los datos en memoria (tabla de procesos)
    QPoint animPosition_;    // Para la animación
};

#endif // MAINWINDOW_H
