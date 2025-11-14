#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include "productioncontroller.h"

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

private slots:
    // Botones
    void on_btnStart_clicked();
    void on_btnStop_clicked();

    // Barras de estadísticas
    void updateProcessedCount(int v);
    void updateActiveWorkers(int v);

    // 👇 Nuevo slot para llenar la tabla
    void onProcessEvent(const QString &station,
                        int productId,
                        const QString &state,
                        const QString &time);

private:
    Ui::MainWindow *ui;
    ProductionController controller_;

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
};

#endif // MAINWINDOW_H
