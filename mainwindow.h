#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QPoint>
#include "productioncontroller.h"
#include "animationmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();

    void updateProcessedCount(int v);
    void updateActiveWorkers(int v);

    void onProcessEvent(const QString &station,
                        int productId,
                        const QString &state,
                        const QString &time);

    // 🔥 Recibimos posición animada
    void onAnimationUpdated(const QPoint &pos);

private:
    Ui::MainWindow *ui;

    ProductionController controller_;
    AnimationManager animationManager_;

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

    // Posición para dibujar la caja animada
    QPoint animPosition_;
};

#endif // MAINWINDOW_H
