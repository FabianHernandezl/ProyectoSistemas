#include "animationmanager.h"
#include <QTimer>
#include <QPainter>
#include <QPixmap>

AnimationManager::AnimationManager(QObject *parent)
    : QObject(parent),
    animationSpeed_(50),  // 50 ms por cuadro de animación
    isAnimating_(false)
{
    animationTimer_ = new QTimer(this);
    connect(animationTimer_, &QTimer::timeout, this, &AnimationManager::onAnimationFrame);
    boxPixmap_ = QPixmap(":/resources/caja.png");
}

AnimationManager::~AnimationManager()
{
    delete animationTimer_;
}

void AnimationManager::startAnimations()
{
    if (!isAnimating_) {
        isAnimating_ = true;
        animationTimer_->start(animationSpeed_);
    }
}

void AnimationManager::stopAnimations()
{
    if (isAnimating_) {
        isAnimating_ = false;
        animationTimer_->stop();
    }
}

void AnimationManager::onAnimationFrame()
{
    // Aquí logicas de movimiento o actualización de las posiciones
    productPosition_ += QPoint(5, 0); // mueve la caja a la derecha por ejemplo
    if (productPosition_.x() > 800) {
        productPosition_.setX(0);  // Resetea la posición si sale de la pantalla
    }

    emit updatePosition(productPosition_);
}

void AnimationManager::updatePosition(int productId, const QPoint& newPosition)
{
    // Actualiza la posición del producto, por ejemplo
    productPosition_ = newPosition;
}

void AnimationManager::paintEvent(QPainter& painter)
{
    painter.drawPixmap(productPosition_, boxPixmap_);
}
