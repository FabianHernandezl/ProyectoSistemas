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
    void updatePosition(int productId, const QPoint& newPosition);

protected:
    void paintEvent(QPainter& painter);

private:
    QTimer *animationTimer_;
    int animationSpeed_;  // Puedes modificar esto para hacer las animaciones más lentas o rápidas
    QPoint productPosition_;
    QPixmap boxPixmap_;
    bool isAnimating_;

private slots:
    void onAnimationFrame();
};

#endif // ANIMATIONMANAGER_H
