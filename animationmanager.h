#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPainter>

class AnimationManager : public QObject
{
    Q_OBJECT

public:
    explicit AnimationManager(QObject *parent = nullptr);
    ~AnimationManager();

    void startAnimations();
    void stopAnimations();

    // ✔ método público para pintarse
    void render(QPainter &painter);

signals:
    void positionChanged(const QPoint &pos);

public slots:
    void updatePosition(int productId, const QPoint &newPosition);

private slots:
    void onAnimationFrame();

private:
    QTimer *animationTimer_;
    int animationSpeed_;
    QPoint productPosition_;
    QPixmap boxPixmap_;
    bool isAnimating_;
    bool isPaused_;              // Para saber si la animación está en pausa
    int pauseDuration_;          // Duración de la pausa en ciclos
    int currentPauseTime_;       // Tiempo actual de la pausa
    QVector<int> pausePositions_; // Vector de posiciones en las que hacer una pausa
    int currentPauseIndex_;      // Índice actual de la pausa que estamos esperando
};

#endif // ANIMATIONMANAGER_H
