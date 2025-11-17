#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>
#include <QCloseEvent>
#include <QMessageBox>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "productioncontroller.h"
#include "animationmanager.h"
#include "animationworkermanager.h"
#include "persistence_manager.h"

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
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:

    // --- 4 BOTONES PRINCIPALES ---
    void on_btnStart_clicked();
    void on_btnPause_clicked();
    void on_btnDelete_clicked();
    void on_btnExit_clicked();

    // --- ESTADÍSTICAS ---
    void updateProcessedCount(int v);
    void updateActiveWorkers(int v);

    // --- EVENTOS PRODUCCIÓN ---
    void onProcessEvent(const QString &station,
                        int productId,
                        const QString &state,
                        const QString &time);

    // --- ANIMACIÓN ---
    void onAnimationUpdated(const QPoint &pos);

    // --- ELIMINACIÓN ---
    void clearAllProduction();
    void deleteSpecificProcess();

private:
    Ui::MainWindow *ui;

    // Controladores
    ProductionController controller_;

    // Animación
    AnimationManager animationManager_;
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

    // Persistencia
    QJsonArray currentRows_;

    // Posición de la caja animada
    QPoint animPosition_;
};

#endif // MAINWINDOW_H
