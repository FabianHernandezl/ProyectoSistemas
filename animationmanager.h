#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPoint>
#include <QPainter>

class AnimationManager : public QObject
{
    Q_OBJECT

public:
    explicit AnimationManager(QObject *parent = nullptr);
    ~AnimationManager();

    void startAnimations();
    void stopAnimations();
    void render(QPainter &painter);
    void updatePosition(int productId, const QPoint &newPos);

signals:
    void positionChanged(const QPoint &pos);

    // 🔥 Señales para cada parada
    void pause1Reached();
    void pause2Reached();
    void pause3Reached();
    void pause4Reached();
    void pause5Reached();

    // Señales para detener cada trabajador por separado
    void stop1Reached();
    void stop2Reached();
    void stop3Reached();
    void stop4Reached();
    void stop5Reached();

    // 🔥 Señal para detener todos los trabajadores
    void allWorkersStop();

private slots:
    void onAnimationFrame();

private:
    QTimer *animationTimer_;
    int animationSpeed_;
    bool isAnimating_;

    QPoint productPosition_;
    QPixmap boxPixmap_;

    // Manejo de pausas
    bool isPaused_;
    int pauseDuration_;
    int currentPauseTime_;
    int currentPauseIndex_;

    QVector<int> pausePositions_;
};

#endif // ANIMATIONMANAGER_H
